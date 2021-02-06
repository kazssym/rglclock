// glgtk.cpp
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "glgdkx.h"

#include <stdexcept>
#include <GL/glx.h>
#include <gdk/gdkx.h>

using std::runtime_error;

namespace glgdkx
{
    glgdkx_context::glgdkx_context (GdkWindow *drawable)
    {
        _display = GDK_WINDOW_XDISPLAY (drawable);
        _context = NULL;

        int error_base, event_base;
        if (!glXQueryExtension (_display, &error_base, &event_base))
        {
            throw runtime_error ("glXQueryExtension");
        }

        int major_version, minor_version;
        if (!glXQueryVersion (_display, &major_version, &minor_version))
        {
            throw runtime_error ("glXQueryVersion");
        }
    }

    glgdkx_context::glgdkx_context (const glgdkx_context &object)
    {
        _display = object._display;
        _context = NULL;
    }

    glgdkx_context::~glgdkx_context ()
    {
        if (_context != NULL)
        {
            glXDestroyContext (_display, _context);
        }
    }

    bool glgdkx_context::make_current (GdkWindow *drawable)
    {
        if (_context == NULL)
        {
            static const int attr[] = {
                GLX_DOUBLEBUFFER, True,
                GLX_RENDER_TYPE, GLX_RGBA_BIT,
                GLX_RED_SIZE,    5,
                GLX_GREEN_SIZE,  5,
                GLX_BLUE_SIZE,   5,
                GLX_DEPTH_SIZE, 16,
                None
            };
            int items;
            GLXFBConfig *config = glXChooseFBConfig (
                _display, DefaultScreen (_display), attr, &items);
            if (config == NULL)
            {
                return false;
            }

            _context = glXCreateNewContext (_display, config[0],
                                            GLX_RGBA_TYPE, NULL, True);
            if (_context == NULL)
            {
                XFree (config);
                return false;
            }

            XFree (config);
        }

        if (drawable == NULL)
        {
            if (!glXMakeCurrent (_display, None, NULL))
            {
                return false;
            }
        }
        else
        {
            if (!glXMakeCurrent (_display, GDK_WINDOW_XID (drawable),
                                 _context))
            {
                return false;
            }
        }

        return true;
    }

    void glgdkx_context::swap_buffers (GdkWindow *drawable)
    {
        glXSwapBuffers (_display, GDK_WINDOW_XID (drawable));
    }
}
