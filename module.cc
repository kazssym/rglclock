/* rglclock - Rotating GL Clock.
   Copyright (C) 1998 Hypercore Software Design, Ltd.

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
   02111-1307, USA.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#undef const

#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <new>

#include "module.h"

#ifdef LOGO128
# include "logo128.h"
#endif

const GLfloat HAND_ADC[4] = {0.1, 0.1, 0.1, 1};
const GLfloat HAND_SC[4] = {0.6, 0.6, 0.6, 1};
const GLfloat HAND_SR = 32;

void
module::draw_clock (const struct tm *tm) const
{
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  gluLookAt (0, 0, 150,
	     0, 0, 0,
	     0, 1, 0);
  glMultMatrixf (rot);

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Use flat shading for the dial disk.  */
  glShadeModel (GL_FLAT);

#ifdef LOGO128
  const GLfloat vs[4] = {0, 0, 0, 1};
#else
  const GLfloat vs[4] = {0.6, 0.6, 0.6, 1.};
#endif
  glMaterialfv (GL_FRONT, GL_SPECULAR, vs);
  glMaterialf (GL_FRONT, GL_SHININESS, 16.);

  {
#ifdef LOGO128
    const GLfloat v[4] = {0.6, 0.6, 0.6, 1.};
#else
    const GLfloat v[4] = {0.1, 0.0, 0.4, 1.};
#endif
    glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, v);
    GLUquadricObj *qobj = gluNewQuadric ();
    if (qobj == NULL)
      throw std::bad_alloc ();
#ifdef LOGO128
    glEnable (GL_TEXTURE_2D);
    gluQuadricTexture (qobj, GL_TRUE);
#endif
    gluDisk (qobj, 0., 45., 24, 1);

    /* Draw the back.  */
    glPushMatrix ();
    glRotatef (180., 0., 1., 0.);
    gluDisk (qobj, 0., 45., 24, 1);
    glPopMatrix ();

#ifdef LOGO128
    glDisable (GL_TEXTURE_2D);
#endif
    gluDeleteQuadric (qobj);
  }

#if 0
  glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
#endif

  glShadeModel (GL_SMOOTH);
  glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, HAND_ADC);
  glMaterialfv (GL_FRONT, GL_SPECULAR, HAND_SC);
  glMaterialf (GL_FRONT, GL_SHININESS, HAND_SR);

  glBegin (GL_QUADS);
  glNormal3f (1., 0., 1.);
  glVertex3f (1., 37., 0.);
  glVertex3f (1., 43., 0.);
  glVertex3f (0., 43., 1.);
  glVertex3f (0., 37., 1.);
  glNormal3f (-1., 0., 1.);
  glVertex3f (0., 37., 1.);
  glVertex3f (0., 43., 1.);
  glVertex3f (-1., 43., 0.);
  glVertex3f (-1., 37., 0.);
  glEnd ();

  /* Short hand.  */
  glPushMatrix ();
  glRotatef (((tm->tm_hour * 60 + tm->tm_min) * 60
	      + tm->tm_sec) / 120., 0, 0, -1);
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
  glRotatef ((tm->tm_min * 60 + tm->tm_sec) / 10., 0, 0, -1);
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

#if 0
  glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
#endif
}

void
module::rotate (double deg,
		double x, double y, double z)
{
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  glRotatef (deg,
	     x, y, z);
  glMultMatrixf (rot);
  glGetFloatv (GL_MODELVIEW_MATRIX, rot);
}

void
module::viewport (int x, int y, int width, int height)
{
  glViewport (x, y, width, height);
}

void
module::init ()
{
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  glGetFloatv (GL_MODELVIEW_MATRIX, rot);
  gluLookAt (0, 0, 150,
	     0, 0, 0,
	     0, 1, 0);

  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  {
    const GLfloat position[] = {-1., 1., 1., 0.};
    glLightfv (GL_LIGHT0, GL_POSITION, position);
  }
  glEnable (GL_LIGHT1);
  {
    const GLfloat position[] = {200., 200., 0., 0.};
    glLightfv (GL_LIGHT1, GL_POSITION, position);
    const GLfloat intensity[] = {0.8, 0.8, 0.8, 1.};
    glLightfv (GL_LIGHT1, GL_DIFFUSE, intensity);
    glLightfv (GL_LIGHT1, GL_SPECULAR, intensity);
  }

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glFrustum (-5., 5., -5., 5., 15., 250.);

  glEnable (GL_DEPTH_TEST);
  glEnable (GL_CULL_FACE);

#ifdef LOGO128
  glTexImage2D (GL_TEXTURE_2D, 0,
		3, 128, 128, 0,
		GL_RGB, GL_UNSIGNED_BYTE,
		data);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif
}

module::~module ()
{
}

module::module ()
{
}
