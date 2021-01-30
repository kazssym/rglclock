/*
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "glgtk.h"

#include <stdexcept>
#include <GL/glx.h>
#include <gdk/gdkx.h>

using std::runtime_error;

namespace glgtk
{
    glgtk_context::glgtk_context (GdkWindow *drawable)
    {
        _display = GDK_DRAWABLE_XDISPLAY (drawable);
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

    glgtk_context::glgtk_context (const glgtk_context &object)
    {
        _display = object._display;
        _context = NULL;
    }

    glgtk_context::~glgtk_context ()
    {
        if (_context != NULL)
        {
            glXDestroyContext (_display, _context);
        }
    }

    bool glgtk_context::make_current (GdkWindow *drawable)
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
            if (!glXMakeCurrent (_display, GDK_DRAWABLE_XID (drawable),
                                 _context))
            {
                return false;
            }
        }

        return true;
    }

    void glgtk_context::swap_buffers (GdkWindow *drawable)
    {
        glXSwapBuffers (_display, GDK_DRAWABLE_XID (drawable));
    }

#if 1
    int glx_error_base;
    int glx_event_base;
    int glx_major_version;
    int glx_minor_version;

    void gl::initialize ()
    {
        static bool initialized;
        if (!initialized)
        {
            Display *d = GDK_DISPLAY ();

            if (!glXQueryExtension (d, &glx_error_base, &glx_event_base))
                throw runtime_error ("glXQueryExtension");
            if (!glXQueryVersion (d, &glx_major_version, &glx_minor_version))
                throw runtime_error ("glXQueryVersion");

            initialized = true;
        }
    }

    GdkVisual *gl::best_visual ()
    {
        static GdkVisual *bv;
        if (bv == 0)
        {
            initialize();

            Display *d = GDK_DISPLAY ();

            int glx_attr[] = {
                GLX_RGBA,
                GLX_DOUBLEBUFFER,
                GLX_DEPTH_SIZE, 12,
                None
            };
            XVisualInfo *xvi = glXChooseVisual (d, DefaultScreen (d), glx_attr);
            if (xvi == NULL)
                throw runtime_error ("glXChooseVisual");

            bv = gdkx_visual_get (xvi->visualid);

            XFree (xvi);
        }

        return bv;
    }

    rglclock::gl_context gl::create_context (GdkVisual *v)
    {
        initialize ();

        Display *d = GDK_DISPLAY ();

        XVisualInfo xvit;
        xvit.visualid = XVisualIDFromVisual (GDK_VISUAL_XVISUAL (v));
        int n;
        XVisualInfo *xvi = XGetVisualInfo (d, VisualIDMask, &xvit, &n);
        if (xvi == 0)
            throw runtime_error ("XGetVisualInfo");

        GLXContext xc = glXCreateContext (d, &xvi[0], 0, true);

        XFree (xvi);

        return reinterpret_cast<rglclock::gl_context> (xc);
    }

    void gl::destroy_context (rglclock::gl_context c)
    {
        if (c != 0)
        {
            Display *d = GDK_DISPLAY ();
            glXDestroyContext (d, reinterpret_cast<GLXContext> (c));
        }
    }

    void gl::make_current (rglclock::gl_context c, GdkWindow *d)
    {
        Display *dd;
        Drawable dw;
        if (c == 0)
        {
            dd = GDK_DISPLAY ();
            dw = None;
        }
        else if (d == 0)
        {
            dd = GDK_DISPLAY ();
            dw = GDK_ROOT_WINDOW ();
        }
        else
        {
            dd = GDK_WINDOW_XDISPLAY (d);
            dw = GDK_WINDOW_XWINDOW (d);
        }

        if (!glXMakeCurrent(dd, dw, reinterpret_cast<GLXContext> (c)))
            throw runtime_error ("glXMakeCurrent");
    }

    void gl::swap_buffers (GdkWindow *d)
    {
        Drawable xd = GDK_WINDOW_XWINDOW (d);

        glXSwapBuffers (GDK_WINDOW_XDISPLAY (d), xd);
    }

#endif
}

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-offsets: ((substatement-open . 0) (arglist-intro . +) (arglist-close . 0))
 * End:
 */
