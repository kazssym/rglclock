// glgdkx.h
// Copyright (C) 1998-2007 Hypercore Software Design, Ltd.
// Copyright (C) 2021 Kaz Nishimura
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

#ifndef GLGDKX_H
#define GLGDKX_H 1

#include <GL/glx.h>
#include <gdk/gdk.h>

namespace glgdkx
{
    class glgdkx_context
    {
    private:
        Display *_display;
        GLXContext _context;

    public:
        explicit glgdkx_context (GdkWindow *drawable);
        glgdkx_context (const glgdkx_context &object);

    public:
        virtual ~glgdkx_context ();
        bool make_current (GdkWindow *drawable);
        void swap_buffers (GdkWindow *drawable);
    };
}

#endif
