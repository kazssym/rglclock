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


static void       gdk_gl_context_init    (GdkGLContext *gdk_gl_context);
static void       gdk_gl_context_destroy (GdkGLContext *gdk_gl_context);
static GLXContext gdk_gl_context_get_glx_context
					 (GdkGLContext *gdk_gl_context);
static int  x_visual_class_from_gdk_visual_type (GdkVisualType);
static void x_visual_info_from_gdk_visual       (XVisualInfo *, GdkVisual *);


static void
gdk_gl_context_init (GdkGLContext *gdk_gl_context)
{
  GdkGLContextPrivate *private;

  g_return_if_fail (gdk_gl_context != NULL);

  private = (GdkGLContextPrivate *)gdk_gl_context;


  private->gdk_gl_context.user_data = NULL;

  private->glx_context = NULL;
  private->gdk_visual = NULL;
  private->ref_count = 0;
}

GdkGLContext*
gdk_gl_context_new (GdkVisual *gdk_visual)
{
  GdkGLContextPrivate *private;

  private = g_new(GdkGLContextPrivate, 1);

  gdk_gl_context_init ((GdkGLContext *)private);

  if (gdk_visual != NULL)
      private->gdk_visual = gdk_visual;

  private->glx_context
      = gdk_gl_context_get_glx_context ((GdkGLContext *)private);

  private->ref_count = 1;

  return (GdkGLContext *)private;
}

static void
gdk_gl_context_destroy (GdkGLContext *gdk_gl_context)
{
  GdkGLContextPrivate *private;

  Display *x_display;

  g_return_if_fail (gdk_gl_context != NULL);
  private = (GdkGLContextPrivate *)gdk_gl_context;
  g_return_if_fail (private->ref_count == 0);


  x_display = (Display *)GDK_DISPLAY(); /* XXX */

  glXDestroyContext (x_display, private->glx_context);

  g_free(private);
}

static GLXContext
gdk_gl_context_get_glx_context (GdkGLContext *gdk_gl_context)
{
  GdkGLContextPrivate *private;

  GLXContext  glx_context;
  Display     *x_display;
  XVisualInfo *x_visual_info;

  g_return_val_if_fail (gdk_gl_context != NULL, NULL);

  private = (GdkGLContextPrivate *)gdk_gl_context;


  if (private->gdk_visual == NULL)
      private->gdk_visual = gdk_visual_get_best();

  x_display = (Display *)GDK_DISPLAY(); /* XXX */

  x_visual_info = g_new(XVisualInfo, 1);
  x_visual_info_from_gdk_visual(x_visual_info, private->gdk_visual);

  /* XXX: glXCreateContext(,,*** shareList ***,) */
  glx_context
      = glXCreateContext (x_display, x_visual_info, None, True );

  g_free(x_visual_info);

  return glx_context;
}

GdkGLContext *
gdk_gl_context_ref (GdkGLContext *gdk_gl_context)
{
  GdkGLContextPrivate *private;
    
  g_return_val_if_fail (gdk_gl_context != NULL, NULL);

  private = (GdkGLContextPrivate *)gdk_gl_context;
  
  private->ref_count += 1;

  return (GdkGLContext *)private;
}

void
gdk_gl_context_unref (GdkGLContext *gdk_gl_context)
{
  GdkGLContextPrivate *private;

  g_return_if_fail (gdk_gl_context != NULL);

  private = (GdkGLContextPrivate *)gdk_gl_context;

  private->ref_count -= 1;

  if (private->ref_count == 0)
    {
      gdk_gl_context_destroy ((GdkGLContext *)private);
    }
}

GdkVisual*
gdk_gl_context_get_visual (GdkGLContext *gdk_gl_context)
{
  GdkGLContextPrivate *private;

  GdkVisual *gdk_visual;

  g_return_val_if_fail (gdk_gl_context != NULL, NULL);


  private = (GdkGLContextPrivate *)gdk_gl_context;

  gdk_visual = private->gdk_visual;

  return gdk_visual;
}


static void
x_visual_info_from_gdk_visual(XVisualInfo *x_visual_info,
			      GdkVisual *gdk_visual)
{
  Display *x_display;

  x_display = (Display *)GDK_DISPLAY();
    
  x_visual_info->visual        = (Visual *)GDK_VISUAL_XVISUAL (gdk_visual);

  /* FIXME XXX */
  x_visual_info->visualid      = XVisualIDFromVisual (x_visual_info->visual);
  x_visual_info->screen        = DefaultScreen (x_display);

  x_visual_info->class         = x_visual_class_from_gdk_visual_type
				      (gdk_visual->type);
  x_visual_info->depth         = gdk_visual->depth;
  x_visual_info->red_mask      = gdk_visual->red_mask;
  x_visual_info->green_mask    = gdk_visual->green_mask;
  x_visual_info->blue_mask     = gdk_visual->blue_mask;
  x_visual_info->colormap_size = gdk_visual->colormap_size;
  x_visual_info->bits_per_rgb  = gdk_visual->bits_per_rgb;
}

static int
x_visual_class_from_gdk_visual_type (GdkVisualType gdk_visual_type)
{
  int x_visual_class;

  x_visual_class = NULL;

  switch (gdk_visual_type)
    {
    case GDK_VISUAL_STATIC_GRAY:
      x_visual_class	= StaticGray;
      break;
    case GDK_VISUAL_GRAYSCALE:
      x_visual_class	= GrayScale;
      break;
    case GDK_VISUAL_STATIC_COLOR:
      x_visual_class	= StaticColor;
      break;
    case GDK_VISUAL_PSEUDO_COLOR:
      x_visual_class	= PseudoColor;
      break;
    case GDK_VISUAL_TRUE_COLOR:
      x_visual_class	= TrueColor;
      break;
    case GDK_VISUAL_DIRECT_COLOR:
      x_visual_class	= DirectColor;
      break;
    }

  return x_visual_class;
}
