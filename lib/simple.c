/* rglclock - Rotating GL Clock.
   Copyright (C) 1998, 2000 Hypercore Software Design, Ltd.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.

   As a special exception, you may copy and distribute this program
   linked with an OpenGL library in executable form without the source
   code for the library, only if such distribution is not prohibited
   by the library's license.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <simple.h>
#include <rglclockmod.h>

#ifdef HAVE_PNG_H
# include <png.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
# ifdef TIME_WITH_SYS_TIME
#  include <time.h>
# endif /* TIME_WITH_SYS_TIME */
#else /* not HAVE_SYS_TIME_H */
# include <time.h>
#endif
#include <stdlib.h>

#include <assert.h>

static const GLfloat vs[4] = {0, 0, 0, 1};
static const GLfloat v[4] = {0.20, 0.20, 0.40, 1.};
static const GLfloat vt[4] = {1.00, 1.00, 1.00, 1.};

static const GLfloat HAND_ADC[4] = {0.05, 0.05, 0.05, 1};
static const GLfloat HAND_SC[4] = {0.20, 0.20, 0.20, 1};
static const GLfloat HAND_SR = 16;

#ifndef DISABLE_LOCAL_VIEWER
# define ENABLE_LOCAL_VIEWER 1
#endif

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

  if (setjmp(png_ptr->jmpbuf) != 0)
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

/* Draws a disk for dial.  If BACK is true, the back face is drawn,
   otherwise the front.  */
static int
draw_dial_disk(int back)
{
  GLUquadricObj *qobj;

  qobj = gluNewQuadric ();
  if (qobj == NULL)
    return -1;

  /* Use flat shading for the dial disk.  */
  glShadeModel(GL_FLAT);
#ifdef ENABLE_LOCAL_VIEWER
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
#endif

  if (texture_mapping && texture_width != 0)
    {
      glEnable(GL_TEXTURE_2D);
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, vt);
      gluQuadricTexture(qobj, GL_TRUE);
    }
  else
    {
      glDisable(GL_TEXTURE_2D);
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, v);
    }

  glMaterialfv(GL_FRONT, GL_SPECULAR, vs);
  glMaterialf(GL_FRONT, GL_SHININESS, 0.);

  if (back)
    gluQuadricOrientation(qobj, GLU_INSIDE);

  gluDisk(qobj, 0., 45., 36, 1);

  gluDeleteQuadric (qobj);
  return 0;
}

int
simple_draw_clock(void)
{
#ifdef HAVE_GETTIMEOFDAY
  struct timeval t;
#else /* not HAVE_GETTIMEOFDAY */
  time_t t;
#endif /* not HAVE_GETTIMEOFDAY */
  const struct tm *lt;

  if (texture_changed)
    {
      texture_changed = 0;
      set_texture();
    }

#ifdef HAVE_GETTIMEOFDAY
  gettimeofday(&t, NULL);
  lt = localtime(&t.tv_sec);
#else /* not HAVE_GETTIMEOFDAY */
  t = time(NULL);
  lt = localtime(&t);
#endif /* not HAVE_GETTIMEOFDAY */

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);
  if (draw_dial_disk(0) == -1)
    return -1;

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, HAND_ADC);
  glMaterialfv (GL_FRONT, GL_SPECULAR, HAND_SC);
  glMaterialf (GL_FRONT, GL_SHININESS, HAND_SR);

  {
    int i;

    glPushMatrix();
    for (i = 0; i != 12; ++i)
      {
	int l;
	if (i == 0)
	  l = 9;
	else if (i % 3 == 0)
	  l = 6;
	else
	  l = 4;
	glBegin (GL_QUADS);
	glNormal3f (0.5, 0., 1.);
	glVertex3f (1., 43. - l, 0.);
	glVertex3f (1., 43., 0.);
	glVertex3f (0., 43., 0.5);
	glVertex3f (0., 43. - l, 0.5);
	glNormal3f (-0.5, 0., 1.);
	glVertex3f (0., 43. - l, 0.5);
	glVertex3f (0., 43., 0.5);
	glVertex3f (-1., 43., 0.);
	glVertex3f (-1., 43. - l, 0.);
	glEnd ();
	glRotatef(30, 0, 0, -1);
      }
    glPopMatrix();
  }

#ifdef ENABLE_LOCAL_VIEWER
  glShadeModel(GL_SMOOTH);
  glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
#endif

  /* Short hand.  */
  glPushMatrix ();
  glRotatef (((lt->tm_hour * 60 + lt->tm_min) * 60
	      + lt->tm_sec) / 120., 0, 0, -1);
  glBegin (GL_TRIANGLES);
  glNormal3f (0.333, 0., 1.);
  glVertex3f (3., 3., 1.);
  glVertex3f (0., 25., 2.);
  glVertex3f (0., 0., 2.);
  glNormal3f (-0.333, 0., 1.);
  glVertex3f (0., 0., 2.);
  glVertex3f (0., 25., 2.);
  glVertex3f (-3., 3., 1.);
  glEnd ();
  glPopMatrix ();

  /* Long hand.  */
  glPushMatrix ();
#ifdef HAVE_GETTIMEOFDAY
  glRotatef ((lt->tm_min * 60 + lt->tm_sec + t.tv_usec / 1e6) / 10., 0, 0, -1);
#else /* not HAVE_GETTIMEOFDAY */
  glRotatef ((lt->tm_min * 60 + lt->tm_sec) / 10., 0, 0, -1);
#endif /* not HAVE_GETTIMEOFDAY */
  glBegin (GL_TRIANGLES);
  glNormal3f (0.5, 0., 1.);
  glVertex3f (2., 2., 3.);
  glVertex3f (0., 40., 4.);
  glVertex3f (0., 0., 4.);
  glNormal3f (-0.5, 0., 1.);
  glVertex3f (0., 0., 4.);
  glVertex3f (0., 40., 4.);
  glVertex3f (-2., 2., 3.);
  glEnd ();
  glPopMatrix ();

  glDisable(GL_DEPTH_TEST);
  if (draw_dial_disk(1) == -1)
    return -1;

  return 0;
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

static const GLfloat LIGHT0_AMBIENT[] = {0.10, 0.10, 0.10, 1};
static const GLfloat LIGHT0_INTENSITY[] = {0.80, 0.80, 0.80, 1};
static const GLfloat LIGHT0_POSITION[] = {-200, 200, 200, 0};
static const GLfloat LIGHT1_AMBIENT[] = {0.10, 0.10, 0.10, 1};
static const GLfloat LIGHT1_INTENSITY[] = {0.60, 0.60, 0.60, 1};
static const GLfloat LIGHT1_POSITION[] = {200, 200, 0, 0};

int
simple_init(void)
{
  /* Sets the projection matrix.  */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glFrustum (-5., 5., -5., 5., 15., 250.);

  glMatrixMode (GL_MODELVIEW);
  gluLookAt (0, 0, 150,
	     0, 0, 0,
	     0, 1, 0);

#if 0
  glEnable (GL_DEPTH_TEST);
#endif
  glEnable (GL_CULL_FACE);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, LIGHT0_AMBIENT);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, LIGHT0_INTENSITY);
  glLightfv(GL_LIGHT0, GL_SPECULAR, LIGHT0_INTENSITY);
  glLightfv(GL_LIGHT0, GL_POSITION, LIGHT0_POSITION);
  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, LIGHT1_AMBIENT);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LIGHT1_INTENSITY);
  glLightfv(GL_LIGHT1, GL_SPECULAR, LIGHT1_INTENSITY);
  glLightfv(GL_LIGHT1, GL_POSITION, LIGHT1_POSITION);

  return 0;
}
