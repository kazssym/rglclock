// simple.c
// Copyright (C) 1998, 2000 Hypercore Software Design, Ltd.
// Copyright (C) 2021-2022 Kaz Nishimura
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <simple.h>
#include <rglclockmod.h>

#include <mat4.h>
#include <png.h>
#include <GL/gl.h>
#include <GL/glu.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
# ifdef TIME_WITH_SYS_TIME
#  include <time.h>
# endif /* TIME_WITH_SYS_TIME */
#else /* not HAVE_SYS_TIME_H */
# include <time.h>
#endif
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <assert.h>

struct simple_module
{
    int dummy;
};

static int texture_mapping = 0;
static char *texture_file = NULL;

static size_t texture_width;
static unsigned char *texture_image;
static int texture_changed = 0;

static png_color_16 background =
{
  0,
  0x3333,
  0x3333,
  0x6666
};

static void
modify_image_info(png_struct *png_ptr, png_info *info_ptr)
{
  int color_type;
  int bit_depth;
  double file_gamma;

  color_type = png_get_color_type(png_ptr, info_ptr);
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);

  if (color_type == PNG_COLOR_TYPE_PALETTE
      || png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_expand(png_ptr);

  if (bit_depth == 16)
    png_set_strip_16(png_ptr);
  else if (bit_depth < 8)
    png_set_packing(png_ptr);

  if (color_type == PNG_COLOR_TYPE_GRAY
      || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr);

  png_set_background(png_ptr, &background,
		     PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.);

  if (!png_get_gAMA(png_ptr, info_ptr, &file_gamma))
    file_gamma = 0.45455;
  png_set_gamma(png_ptr, 1., file_gamma);

  png_read_update_info(png_ptr, info_ptr);
}

static void
read_texture_image(png_struct *png_ptr, png_info *info_ptr)
{
  size_t image_width, image_height;
  size_t w;
  size_t texture_row_size, texture_size;
  unsigned char *p;
  png_bytep *rows, *q;

  image_width = png_get_image_width(png_ptr, info_ptr);
  image_height = png_get_image_height(png_ptr, info_ptr);

  w = 1;
  while (w < image_width || w < image_height)
    {
      w *= 2;
      assert(w > 0);
    }

  texture_row_size = w * 3;
  texture_size = w * texture_row_size;

  texture_width = w;
  texture_image = realloc(texture_image, texture_size);

  for (p = texture_image; p != texture_image + texture_size; p += 3)
    {
      p[0] = background.red >> 8;
      p[1] = background.green >> 8;
      p[2] = background.blue >> 8;
    }

  rows = malloc(image_height * sizeof (png_bytep));
  p = (texture_image
       + (texture_width - image_height) / 2 * texture_row_size
       + (texture_width - image_width) / 2 * 3);
  q = rows + image_height;
  while (q != rows)
    {
      *--q = (png_bytep) p;
      p += texture_row_size;
    }

  png_read_image(png_ptr, rows);

  free(rows);
}

static void
load_texture_image(void)
{
  FILE *fp;
  png_struct *png_ptr;
  png_info *info_ptr;

  texture_width = 0;

  fp = fopen(texture_file, "rb");
  if (fp == NULL)
    {
      return;
    }

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
				   NULL, NULL, NULL);
  if (png_ptr == NULL)
    {
      goto error_return_1;
    }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
    {
      goto error_return_2;
    }

  if (setjmp(png_jmpbuf(png_ptr)) != 0)
    {
      goto error_return_2;
    }

  png_init_io(png_ptr,fp);
  png_read_info(png_ptr, info_ptr);
  modify_image_info(png_ptr, info_ptr);

  read_texture_image(png_ptr, info_ptr);

 error_return_2:
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
 error_return_1:
  fclose(fp);
}

static void
set_texture()
{
  glTexImage2D(GL_TEXTURE_2D, 0, 3,
	       texture_width, texture_width, 0,
	       GL_RGB, GL_UNSIGNED_BYTE, texture_image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

int
simple_set_prop(const char *name, const char *value)
{
  if (strcmp(name, "texture_mapping") == 0)
    {
      texture_mapping = atoi(value);
    }
  else if (strcmp(name, "texture_file") == 0)
    {
      texture_file = realloc(texture_file, strlen(value) + 1);
      strcpy(texture_file, value);
      texture_changed = 1;
      load_texture_image();

      return 0;
    }

  return -1;
}

enum {
    BUFFER_MAX = 1,
    VERTEX_ARRAY_MAX = 1,
};

enum vertex_attrib {
    VERTEX,
    NORMAL,
    MATERIAL_AMBIENT,
    MATERIAL_DIFFUSE,
    MATERIAL_SPECULAR,
    MATERIAL_SHININESS,
};

static GLuint vertex_shader;
static GLuint fragment_shader;

static GLuint shader_program;

// OpenGL buffers.
static GLuint buffers[BUFFER_MAX];

// OpenGL vertex arrays.
static GLuint vertex_arrays[VERTEX_ARRAY_MAX];

static void check_gl_errors(const char *file, unsigned int line)
{
    for (;;) {
        GLenum error = glGetError();
        if (error == GL_NO_ERROR) {
            break;
        }
        fprintf(stderr, "%s:%u: GL error: %s\n", file, line, gluErrorString(error));
    }
}

static GLuint compile_shader(GLenum type, GLsizei count,
    const GLchar *const *string, const GLint *length)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, count, string, length);
    glCompileShader(shader);

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint info_log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
        char *info_log = alloca(info_log_length);
        glGetShaderInfoLog(shader, info_log_length, NULL, info_log);
        fprintf(stderr, "GLSL shader compile error\n%s", info_log);
        goto BAILOUT;
    }

    return shader;

BAILOUT:
    glDeleteShader(shader);
    return 0;
}

static GLuint compile_vertex_shader(void)
{
    const char *source =
        "#version 140\n"
        "const int LIGHT_MAX = 2;"
        "in vec4 vertex;\n"
        "in vec3 normal;\n"
        "in vec4 materialAmbient;\n"
        "in vec4 materialDiffuse;\n"
        "in vec4 materialSpecular;\n"
        "in float materialShininess;\n"
        "out vec4 color;\n"
        "uniform mat4 modelMatrix;\n"
        "uniform mat4 viewMatrix;\n"
        "uniform mat4 projectionMatrix;\n"
        "uniform vec4 lightAmbient[LIGHT_MAX];\n"
        "uniform vec4 lightDiffuse[LIGHT_MAX];\n"
        "uniform vec4 lightSpecular[LIGHT_MAX];\n"
        "uniform vec4 lightPosition[LIGHT_MAX];\n"
        "mat4 modelViewMatrix = viewMatrix * modelMatrix;\n"
        "mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));\n"
        "void main()\n"
        "{\n"
        "    vec4 v = modelViewMatrix * vertex;\n"
        "    vec3 n = normalize(normalMatrix * normal);\n"
        "    color = vec4(0, 0, 0, 0);\n"
        "    for (int i = 0; i != LIGHT_MAX; i++) {\n"
        "        vec3 l = normalize(vec3(lightPosition[i]));\n"
        "        vec3 h = normalize(l - normalize(vec3(v)));\n"
        "        color += lightAmbient[i] * materialAmbient;\n"
        "        color += max(dot(l, n), 0) * lightDiffuse[i] * materialDiffuse[i];\n"
        "        color += pow(max(dot(h, n), 0), materialShininess) * lightSpecular[i] * materialSpecular[i];\n"
        "    }\n"
        "    gl_Position = projectionMatrix * v;\n"
        "}\n";
    return compile_shader(GL_VERTEX_SHADER, 1, &source, NULL);
}

static GLuint compile_fragment_shader(void)
{
    const char *source =
        "#version 140\n"
        "in vec4 color;\n"
        "out vec4 fragColor;\n"
        "void main()\n"
        "{\n"
        "    fragColor = color;\n"
        "}\n";
    return compile_shader(GL_FRAGMENT_SHADER, 1, &source, NULL);
}

static GLuint link_shader_program(void)
{
    vertex_shader = compile_vertex_shader();
    fragment_shader = compile_fragment_shader();

    GLuint program = glCreateProgram();
    if (!vertex_shader || !fragment_shader) {
        goto BAILOUT;
    }

    glBindAttribLocation(program, VERTEX, "vertex");
    glBindAttribLocation(program, NORMAL, "normal");
    glBindAttribLocation(program, MATERIAL_AMBIENT, "materialAmbient");
    glBindAttribLocation(program, MATERIAL_DIFFUSE, "materialDiffuse");
    glBindAttribLocation(program, MATERIAL_SPECULAR, "materialSpecular");
    glBindAttribLocation(program, MATERIAL_SHININESS, "materialShininess");

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        GLint info_log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
        char *info_log = alloca(info_log_length);
        glGetProgramInfoLog(program, info_log_length, NULL, info_log);
        fprintf(stderr, "GLSL program link error\n%s", info_log);
        goto BAILOUT;
    }

    return program;

BAILOUT:
    glDeleteProgram(program);
    glDeleteShader(fragment_shader);
    fragment_shader = 0;
    glDeleteShader(vertex_shader);
    vertex_shader = 0;
    return 0;
}

// Initializes the vertex and fragment shaders.
static void init_shaders(void)
{
    shader_program = link_shader_program();
    glUseProgram(shader_program);

    check_gl_errors(__FILE__, __LINE__);
}

// Initializes the buffers.
static void init_buffers(void)
{
    glGenBuffers(BUFFER_MAX, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, 0x2000, NULL, GL_DYNAMIC_DRAW);

    check_gl_errors(__FILE__, __LINE__);
}

// Initializes the vertex arrays.
static void init_vertex_arrays(void)
{
    glGenVertexArrays(VERTEX_ARRAY_MAX, vertex_arrays);
    glBindVertexArray(vertex_arrays[0]);

    check_gl_errors(__FILE__, __LINE__);
}

// Sets the model matrix.
static void set_model_matrix(const GLfloat matrix[4][4])
{
    GLint matrix_location = glGetUniformLocation(shader_program, "modelMatrix");
    glUniformMatrix4fv(matrix_location, 1, GL_FALSE, &matrix[0][0]);

    check_gl_errors(__FILE__, __LINE__);
}

// Sets the view matrix.
static void set_view_matrix(const GLfloat matrix[4][4])
{
    GLint location = glGetUniformLocation(shader_program, "viewMatrix");
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);

    check_gl_errors(__FILE__, __LINE__);
}

// Sets the projection matrix.
static void set_projection_matrix(const GLfloat matrix[4][4])
{
    GLint location = glGetUniformLocation(shader_program, "projectionMatrix");
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);

    check_gl_errors(__FILE__, __LINE__);
}

// Initializes the projection matrix.
static void init_projection_matrix(void)
{
    // As glFrustum(-5, 5, -5, 5, 15, 250)
    static const GLfloat left = -5;
    static const GLfloat right = 5;
    static const GLfloat bottom = -5;
    static const GLfloat top = 5;
    static const GLfloat nearVal = 15;
    static const GLfloat farVal = 250;

    // Note this is a column-major matrix.
    static const GLfloat matrix[4][4] = {
        {2 * nearVal / (right - left), 0, 0, 0},
        {0, 2 * nearVal / (top - bottom), 0, 0},
        {(right + left) / (right - left), (top + bottom) / (top - bottom),
            -(farVal + nearVal) / (farVal - nearVal), -1},
        {0, 0, -2 * farVal * nearVal / (farVal - nearVal), 0},
    };
    set_projection_matrix(matrix);
}

// Initializes the view matrix.
static void init_view_matrix(void)
{
    // As gluLookAt(0, 0, 150, 0, 0, 0, 0, 1, 0)
    // Note this is a column-major matrix.
    static const GLfloat matrix[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, -150, 1},
    };
    set_view_matrix(matrix);
}

static void init_lights(void)
{
    enum {
        LIGHT_MAX = 2,
    };
    static const GLfloat ambient[LIGHT_MAX][4] = {
        {0.08F, 0.08F, 0.08F, 1},
        {0.06F, 0.06F, 0.06F, 1},
    };
    static const GLfloat diffuse[LIGHT_MAX][4] = {
        {0.80F, 0.80F, 0.80F, 1},
        {0.60F, 0.60F, 0.60F, 1},
    };
    static const GLfloat specular[LIGHT_MAX][4] = {
        {1.00F, 1.00F, 1.00F, 1},
        {1.00F, 1.00F, 1.00F, 1},
    };
    static const GLfloat position[LIGHT_MAX][4] = {
        {-200, 200, 200, 0},
        { 200, 200,   0, 0},
    };

    GLint ambient_location = glGetUniformLocation(shader_program, "lightAmbient");
    glUniform4fv(ambient_location, LIGHT_MAX, &ambient[0][0]);

    GLint diffuse_location = glGetUniformLocation(shader_program, "lightDiffuse");
    glUniform4fv(diffuse_location, LIGHT_MAX, &diffuse[0][0]);

    GLint specular_location = glGetUniformLocation(shader_program, "lightSpecular");
    glUniform4fv(specular_location, LIGHT_MAX, &specular[0][0]);

    GLint position_location = glGetUniformLocation(shader_program, "lightPosition");
    glUniform4fv(position_location, LIGHT_MAX, &position[0][0]);

    check_gl_errors(__FILE__, __LINE__);
}

static void rotate_z(const GLfloat model_matrix[4][4], GLfloat angle)
{
    GLfloat rotation[4][4];
    mat4_rotate(angle, 0, 0, -1, rotation);

    GLfloat m[4][4];
    mat4_multiply(model_matrix, (const GLfloat (*)[4])rotation, m);

    set_model_matrix((const GLfloat (*)[4])m);
}

// Draws the tick marks.
static void draw_tick_marks(const GLfloat model_matrix[4][4])
{
    static const GLfloat ambient[4] = {0.05F, 0.05F, 0.05F, 1};
    static const GLfloat diffuse[4] = {0.05F, 0.05F, 0.05F, 1};
    static const GLfloat specular[4] = {1.00F, 1.00F, 1.00F, 1};
    static const GLfloat shininess = 16;

    glVertexAttrib4fv(MATERIAL_AMBIENT, &ambient[0]);
    glVertexAttrib4fv(MATERIAL_DIFFUSE, &diffuse[0]);
    glVertexAttrib4fv(MATERIAL_SPECULAR, &specular[0]);
    glVertexAttrib1f(MATERIAL_SHININESS, shininess);

    for (int i = 0; i != 12; ++i) {
        GLfloat angle = (GLfloat)M_PI / 6 * (GLfloat)i;
        rotate_z(model_matrix, angle);

        GLfloat l = 4;
        if (i == 0) {
            l = 9;
        }
        else if (i % 3 == 0) {
            l = 6;
        }

        const GLfloat vertices[8][4] = {
            { 1, 43 - l, 0,    1},
            { 1, 43,     0,    1},
            { 0, 43,     0.5F, 1},
            { 0, 43 - l, 0.5F, 1},
            { 0, 43 - l, 0.5F, 1},
            { 0, 43,     0.5F, 1},
            {-1, 43,     0,    1},
            {-1, 43 - l, 0,    1},
        };
        const GLfloat normals[8][3] = {
            { 0.5F, 0, 1},
            { 0.5F, 0, 1},
            { 0.5F, 0, 1},
            { 0.5F, 0, 1},
            {-0.5F, 0, 1},
            {-0.5F, 0, 1},
            {-0.5F, 0, 1},
            {-0.5F, 0, 1},
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof vertices, vertices);
        glBufferSubData(GL_ARRAY_BUFFER, 256, sizeof normals, normals);
        glVertexAttribPointer(VERTEX, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
        glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void *)256);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    }

    check_gl_errors(__FILE__, __LINE__);
}

// Draws the short hand.
static void draw_short_hand(const GLfloat model_matrix[4][4], const struct tm *t)
{
    static const GLfloat ambient[4] = {0.05F, 0.05F, 0.05F, 1};
    static const GLfloat diffuse[4] = {0.05F, 0.05F, 0.05F, 1};
    static const GLfloat specular[4] = {1.00F, 1.00F, 1.00F, 1};
    static const GLfloat shininess = 16;

    glVertexAttrib4fv(MATERIAL_AMBIENT, &ambient[0]);
    glVertexAttrib4fv(MATERIAL_DIFFUSE, &diffuse[0]);
    glVertexAttrib4fv(MATERIAL_SPECULAR, &specular[0]);
    glVertexAttrib1f(MATERIAL_SHININESS, shininess);

    GLfloat angle = (GLfloat)M_PI / 21600
        * (GLfloat)((t->tm_hour * 60 + t->tm_min) * 60 + t->tm_sec);
    rotate_z(model_matrix, angle);

    const GLfloat vertices[6][4] = {
        { 2,  0, 1, 1},
        { 0, 25, 2, 1},
        { 0, -2, 2, 1},
        { 0, -2, 2, 1},
        { 0, 25, 2, 1},
        {-2,  0, 1, 1},
    };
    const GLfloat normals[6][3] = {
        { 0.5F, 0, 1},
        { 0.5F, 0, 1},
        { 0.5F, 0, 1},
        {-0.5F, 0, 1},
        {-0.5F, 0, 1},
        {-0.5F, 0, 1},
    };
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof vertices, vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 256, sizeof normals, normals);
    glVertexAttribPointer(VERTEX, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void *)256);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    check_gl_errors(__FILE__, __LINE__);
}

// Draws the long hand.
static void draw_long_hand(const GLfloat model_matrix[4][4], const struct tm *t)
{
    static const GLfloat ambient[4] = {0.05F, 0.05F, 0.05F, 1};
    static const GLfloat diffuse[4] = {0.05F, 0.05F, 0.05F, 1};
    static const GLfloat specular[4] = {1.00F, 1.00F, 1.00F, 1};
    static const GLfloat shininess = 16;

    glVertexAttrib4fv(MATERIAL_AMBIENT, &ambient[0]);
    glVertexAttrib4fv(MATERIAL_DIFFUSE, &diffuse[0]);
    glVertexAttrib4fv(MATERIAL_SPECULAR, &specular[0]);
    glVertexAttrib1f(MATERIAL_SHININESS, shininess);

    GLfloat angle = (GLfloat)M_PI / 1800 * (GLfloat)(t->tm_min * 60 + t->tm_sec);
    rotate_z(model_matrix, angle);

    const GLfloat vertices[6][4] = {
        { 2,  0, 3, 1},
        { 0, 40, 4, 1},
        { 0, -2, 4, 1},
        { 0, -2, 4, 1},
        { 0, 40, 4, 1},
        {-2,  0, 3, 1},
    };
    const GLfloat normals[6][3] = {
        { 0.5F, 0, 1},
        { 0.5F, 0, 1},
        { 0.5F, 0, 1},
        {-0.5F, 0, 1},
        {-0.5F, 0, 1},
        {-0.5F, 0, 1},
    };
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof vertices, vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 256, sizeof normals, normals);
    glVertexAttribPointer(VERTEX, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void *)256);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    check_gl_errors(__FILE__, __LINE__);
}

// Draws the base.
static void draw_base(const GLfloat model_matrix[4][4], bool back)
{
    static const GLfloat ambient[4] = {0.20F, 0.20F, 0.40F, 1};
    static const GLfloat diffuse[4] = {0.20F, 0.20F, 0.40F, 1};
    static const GLfloat specular[4] = {0.40F, 0.40F, 0.40F, 1};
    static const GLfloat shininess = 16;

    glVertexAttrib4fv(MATERIAL_AMBIENT, &ambient[0]);
    glVertexAttrib4fv(MATERIAL_DIFFUSE, &diffuse[0]);
    glVertexAttrib4fv(MATERIAL_SPECULAR, &specular[0]);
    glVertexAttrib1f(MATERIAL_SHININESS, shininess);

    set_model_matrix(model_matrix);

    enum {
        N = 36,
    };
    GLfloat vertices[N + 2][4] = {
        {0, 0, 0, 1},
    };
    GLfloat normals[N + 2][3] = {
        {0, 0, back ? -1 : 1},
    };

    for (int i = 0; i != N + 1; i++) {
        GLfloat angle = -2 * (GLfloat)M_PI / N * i * (back ? -1 : 1);
        vertices[i + 1][0] = 45 * sinf(angle);
        vertices[i + 1][1] = 45 * cosf(angle);
        vertices[i + 1][2] =  0;
        vertices[i + 1][3] =  1;
        normals[i + 1][0] = 0;
        normals[i + 1][1] = 0;
        normals[i + 1][2] = back ? -1 : 1;
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof vertices, vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 0x800, sizeof normals, normals);
    glVertexAttribPointer(VERTEX, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void *)0x800);

    glDrawArrays(GL_TRIANGLE_FAN, 0, N + 2);

    check_gl_errors(__FILE__, __LINE__);
}

int simple_init(void)
{
    init_shaders();
    init_buffers();
    init_vertex_arrays();

    init_projection_matrix();
    init_view_matrix();

    init_lights();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glEnableVertexAttribArray(VERTEX);
    glEnableVertexAttribArray(NORMAL);

    check_gl_errors(__FILE__, __LINE__);

    return 0;
}

int
simple_draw_clock(const GLfloat model_matrix[4][4])
{
    if (texture_changed) {
        texture_changed = 0;
        set_texture();
    }

    time_t t = time(NULL);
#if _POSIX_VERSION
    struct tm lt[1];
    localtime_r(&t, lt);
#else
    struct tm *lt = localtime(&t);
#endif

    check_gl_errors(__FILE__, __LINE__);

#if 0
    GLint matrix_location = glGetUniformLocation(shader_program, "modelMatrix");
    glUniformMatrix4fv(matrix_location, 1, GL_FALSE, model_matrix);
#endif

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    check_gl_errors(__FILE__, __LINE__);

    draw_tick_marks(model_matrix);
    draw_short_hand(model_matrix, lt);
    draw_long_hand(model_matrix, lt);

    draw_base(model_matrix, false);
    draw_base(model_matrix, true);

    return 0;
}

static int
simple_draw(void *data, const GLfloat model_matrix[4][4])
{
    return simple_draw_clock(model_matrix);
}

static int
simple_set_up(void *data)
{
  return simple_init();
}

static int
simple_get_property(void *data, const char *name, char *value, size_t n)
{
  return 0;
}

static int
simple_set_property(void *data, const char *name, const char *value)
{
  return simple_set_prop(name, value);
}

static int
simple_get_property_names(void *data, const char **names, size_t n)
{
  names[0] = NULL;
  return 0;
}

static int
simple_close(void *data)
{
  struct simple_module *module = (struct simple_module *) data;

  free(module);
  return 0;
}

static int
simple_open(void **data)
{
  struct simple_module *module;

  module = malloc(sizeof (struct simple_module));
  if (module == NULL)
    return -1;

  *data = module;
  return 0;
}

static struct rglclockmod_version_1 simple_version_1
= {&simple_open,
   &simple_close,
   &simple_set_up,
   &simple_draw,
   &simple_get_property_names,
   &simple_get_property,
   &simple_set_property};

#define VERSION_CUR 1

int
simple_LTX_query_interface(const char *iname, int version_max,
		union rglclockmod_interface *iface)
{
  if (strcmp(iname, RGLCLOCK_INTERFACE_NAME) != 0
      || version_max < VERSION_CUR)
    return -1;

  iface->version_1 = &simple_version_1;
  return VERSION_CUR;
}
