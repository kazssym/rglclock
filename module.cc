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
   02111-1307, USA.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#undef const

#include "module.h"

#include "simple.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <time.h>
#include <new>

/* Draws a clock.  */
void
module::draw_clock (const struct tm *tm) const
{
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  gluLookAt (0, 0, 150,
	     0, 0, 0,
	     0, 1, 0);
  glMultMatrixf (rot);

  if (simple_draw_clock() == -1)
    throw std::bad_alloc();
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

  simple_init();
}

module::~module ()
{
}

module::module ()
{
}
