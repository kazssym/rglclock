/* gdkGL
 * Copyright (C) 1997,1998 AOSASA Shigeru
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <gdk/gdkx.h>

#include "gdkGL.h"
#include "gdkGLprivate.h"


static GdkGLContext *gdk_gl_current_gdk_gl_context = NULL;
static GdkDrawable  *gdk_gl_current_gdk_drawable = NULL;


gint
gdk_gl_set_current(GdkGLContext *gdk_gl_context,
		   GdkDrawable  *gdk_drawable)
{
  GdkGLContextPrivate *private;

  Display *x_display;
  Window   x_drawable;

  Bool r;

  g_return_val_if_fail (gdk_gl_context != NULL, FALSE);
  g_return_val_if_fail (gdk_drawable != NULL, FALSE);


  private = (GdkGLContextPrivate *)gdk_gl_context;

  x_display  = GDK_WINDOW_XDISPLAY(gdk_drawable);
  x_drawable = GDK_WINDOW_XWINDOW(gdk_drawable);

  r = glXMakeCurrent(x_display, x_drawable, private->glx_context);

  if (r)
    {
      gdk_gl_current_gdk_gl_context = (GdkGLContext *)private;
      gdk_gl_current_gdk_drawable = gdk_drawable;

      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

void
gdk_gl_unset_current (void)
{
  gdk_gl_current_gdk_gl_context = NULL;
  gdk_gl_current_gdk_drawable = NULL;
}

GdkGLContext *
gdk_gl_get_current_context(void)
{
  GLXContext current_glx_context;
  GLXContext real_current_glx_context;
    
  g_return_val_if_fail(gdk_gl_current_gdk_gl_context != NULL, NULL);


  current_glx_context
      = ((GdkGLContextPrivate *)gdk_gl_current_gdk_gl_context)->glx_context;
  real_current_glx_context = glXGetCurrentContext();

  if (current_glx_context != real_current_glx_context)
    {
      return NULL;
    }
  else
    {
      return gdk_gl_current_gdk_gl_context;
    }
}

GdkDrawable *
gdk_gl_get_current_drawable(void)
{
  Window current_x_drawable;
  Window real_current_x_drawable;
    
  g_return_val_if_fail(gdk_gl_current_gdk_drawable != NULL, NULL);


  current_x_drawable = GDK_WINDOW_XWINDOW(gdk_gl_current_gdk_drawable);
  real_current_x_drawable = glXGetCurrentDrawable();

  if (current_x_drawable != real_current_x_drawable)
    {
      return NULL;
    }
  else
    {
      return gdk_gl_current_gdk_drawable;
    }
}

void
gdk_gl_swap_buffers(GdkDrawable *gdk_drawable)
{
  Display      *x_display;
  Window       x_drawable;

  g_return_if_fail(gdk_drawable != NULL);


  x_display = GDK_WINDOW_XDISPLAY(gdk_drawable);
  x_drawable = GDK_WINDOW_XWINDOW(gdk_drawable);

  glXSwapBuffers(x_display, x_drawable);
}

void
gdk_gl_flush_gdk(void)
{
  glXWaitX();
}

void
gdk_gl_flush_gl(void)
{
  glXWaitGL();
}
