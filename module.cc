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

#define _GNU_SOURCE 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#undef const

#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <new>

#include "module.h"

void
module::draw_clock (const struct tm *tm) const
{
  glMatrixMode (GL_MODELVIEW);
#if 0
  glLoadIdentity ();
  gluLookAt (0, 0, 100,
	     0, 0, 0,
	     0, 1, 0);
#endif

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const GLfloat vs[4] = {0.6, 0.6, 0.6, 1.};
  glMaterialfv (GL_FRONT, GL_SPECULAR, vs);
  glMaterialf (GL_FRONT, GL_SHININESS, 16.);

  {
    const GLfloat v[4] = {0.1, 0.0, 0.4, 1.};
    glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, v);
    GLUquadricObj *qobj = gluNewQuadric ();
    if (qobj == NULL)
      throw std::bad_alloc ();
    gluDisk (qobj, 0., 45., 24, 2);

    /* Draw the back.  */
    glPushMatrix ();
    glRotatef (180., 0., 1., 0.);
    gluDisk (qobj, 0., 45., 24, 1);
    glPopMatrix ();

    gluDeleteQuadric (qobj);
  }

  const GLfloat vd[4] = {0.2, 0.2, 0.2, 1.};
  glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, vd);

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
}

void
module::rotate (double deg,
		double x, double y, double z)
{
  glMatrixMode (GL_MODELVIEW);
  glRotated (deg,
	     x, y, z);
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
}

module::~module ()
{
}

module::module ()
{
}
