/* -*-C++-*- */
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
   02111-1307, USA.

   As a special exception, you may copy and distribute this program
   linked with an OpenGL library in executable form without the source
   code for the library, only if such distribution is not prohibited
   by the library's license.  */

#ifndef module_h
#define module_h 1

#include <GL/gl.h>
#include <string>

/* Class wrapper for a clock module.  This class will eventually be
   extended to support dynamically loadable clock modules.  */
class module
{
public:
  module ();
  ~module ();
public:
  void init ();
  void viewport (int, int, int, int);
  void rotate (double, double, double, double);

  /* Draws a clock in the current context.  */
  void draw_clock() const;

  /* Returns the value of property NAME.  */
  std::string prop(const std::string &name) const;

  /* Sets property NAME to VALUE.  */
  void set_prop(const std::string &name, const std::string &value);
  
private:
  GLfloat rot[16];
};

#endif /* not module_h */
