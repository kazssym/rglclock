/* rglclock - Rotating GL Clock.
   Copyright (C) 1998, 1999 Hypercore Software Design, Ltd.

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

#include "simple.h"

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

#ifdef LOGO128
# include "logo128.h"
#endif

static const GLfloat vs[4] = {0, 0, 0, 1};
#ifdef LOGO128
static const GLfloat v[4] = {0.60, 0.60, 0.60, 1.};
#else
static const GLfloat v[4] = {0.10, 0.10, 0.40, 1.};
#endif

static const GLfloat HAND_ADC[4] = {0.05, 0.05, 0.10, 1};
static const GLfloat HAND_SC[4] = {0.40, 0.40, 0.40, 1};
static const GLfloat HAND_SR = 16;

#ifndef DISABLE_LOCAL_VIEWER
# define ENABLE_LOCAL_VIEWER 1
#endif

int
simple_draw_clock(void)
{
#ifdef HAVE_GETTIMEOFDAY
  struct timeval t;
#else /* not HAVE_GETTIMEOFDAY */
  time_t t;
#endif /* not HAVE_GETTIMEOFDAY */
  const struct tm *lt;

#ifdef HAVE_GETTIMEOFDAY
  gettimeofday(&t, NULL);
  lt = localtime(&t.tv_sec);
#else /* not HAVE_GETTIMEOFDAY */
  t = time(NULL);
  lt = localtime(&t);
#endif /* not HAVE_GETTIMEOFDAY */

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  {
    GLUquadricObj *qobj;

    qobj = gluNewQuadric ();
    if (qobj == NULL)
      return -1;

    /* Use flat shading for the dial disk.  */
    glShadeModel (GL_FLAT);
#ifdef ENABLE_LOCAL_VIEWER
    glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
#endif

    glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, v);
    glMaterialfv (GL_FRONT, GL_SPECULAR, vs);
    glMaterialf (GL_FRONT, GL_SHININESS, 0.);

#ifdef LOGO128
    glEnable (GL_TEXTURE_2D);
    gluQuadricTexture (qobj, GL_TRUE);
#endif
    gluDisk (qobj, 0., 45., 36, 1);

    /* Draw the back.  */
    glPushMatrix ();
    glRotatef (180., 0., 1., 0.);
    gluDisk (qobj, 0., 45., 36, 1);
    glPopMatrix ();

#ifdef LOGO128
    glDisable (GL_TEXTURE_2D);
#endif
    gluDeleteQuadric (qobj);
  }

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

  return 0;
}

static const GLfloat LIGHT0_POSITION[] = {-200, 200, 200, 0};
static const GLfloat LIGHT0_INTENSITY[] = {0.80, 0.80, 0.80, 1};
static const GLfloat LIGHT1_POSITION[] = {200, 200, 0, 0};
static const GLfloat LIGHT1_INTENSITY[] = {0.60, 0.60, 0.60, 1};

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

  glEnable (GL_DEPTH_TEST);
  glEnable (GL_CULL_FACE);

  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  glLightfv (GL_LIGHT0, GL_POSITION, LIGHT0_POSITION);
  glLightfv (GL_LIGHT0, GL_DIFFUSE, LIGHT0_INTENSITY);
  glLightfv (GL_LIGHT0, GL_SPECULAR, LIGHT0_INTENSITY);
  glEnable (GL_LIGHT1);
  glLightfv (GL_LIGHT1, GL_POSITION, LIGHT1_POSITION);
  glLightfv (GL_LIGHT1, GL_DIFFUSE, LIGHT1_INTENSITY);
  glLightfv (GL_LIGHT1, GL_SPECULAR, LIGHT1_INTENSITY);

#ifdef LOGO128
  glTexImage2D (GL_TEXTURE_2D, 0,
		3, 128, 128, 0,
		GL_RGB, GL_UNSIGNED_BYTE,
		data);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif

  return 0;
}

