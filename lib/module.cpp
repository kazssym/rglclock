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
#undef const

#include "module.h"

#include "simple.h"
#include <rglclockmod.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <time.h>
#include <new>

using namespace std;

void
module::draw_clock() const
{
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix();

  /* Rotates the model view matrix.  */
  glMultMatrixf (rot);

  if (simple_draw_clock() == -1)
    {
      glPopMatrix();
      throw std::bad_alloc();
    }

  glPopMatrix();
}

void
module::rotate (double deg,
		double x, double y, double z)
{
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix();

  glLoadIdentity ();
  glRotatef (deg,
	     x, y, z);
  glMultMatrixf (rot);
  glGetFloatv (GL_MODELVIEW_MATRIX, rot);

  glPopMatrix();
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

  simple_init();
}

string
module::prop(const string &name) const
{
  return "";
}

void
module::set_prop(const string &name, const string &value)
{
  simple_set_prop(name.c_str(), value.c_str());
}

module::~module ()
{
}

module::module ()
{
}
