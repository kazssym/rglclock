/* -*-C++-*- */
/* rglclock - Rotating GL Clock.
   Copyright (C) 1998-2002 Hypercore Software Design, Ltd.

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

#ifndef GDK_GL_H
#define GDK_GL_H 1

#include "gl_context.h"
#include <gdk/gdktypes.h>

namespace rglclock
{
  class gdk_gl
  {
  protected:
    static void initialize();

  public:
    static GdkVisual *best_visual();
    static gl_context create_context(GdkVisual *v);
    static void destroy_context(gl_context c);
    static void make_current(gl_context c, GdkDrawable *d);
    static void swap_buffers(GdkDrawable *d);
  };
}

#endif /* not GDK_GL_H */
