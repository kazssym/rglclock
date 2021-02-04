/* -*- C++ -*-
 * RGLClock - rotating 3D clock
 * Copyright (C) 1998, 1999, 2000, 2002, 2007 Hypercore Software
 * Design, Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 *
 */

#ifndef glgtkH
#define glgtkH 1

#include "gl_context.h"
#include <GL/glx.h>
#include <gdk/gdk.h>

namespace glgtk
{
    class glgtk_context
    {
    private:
        Display *_display;
        GLXContext _context;

    public:
        explicit glgtk_context (GdkWindow *drawable);
        glgtk_context (const glgtk_context &object);

    public:
        virtual ~glgtk_context ();
        bool make_current (GdkWindow *drawable);
        void swap_buffers (GdkWindow *drawable);
    };

    class gl
    {
    protected:
        static void initialize ();

    public:
        static GdkVisual *best_visual ();
        static rglclock::gl_context create_context (GdkVisual *v);
        static void destroy_context (rglclock::gl_context c);
        static void make_current (rglclock::gl_context c, GdkWindow *d);
        static void swap_buffers (GdkWindow *d);
    };
}

#endif

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-offsets: ((substatement-open . 0) (arglist-intro . +) (arglist-close . 0))
 * End:
 */
