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

#include "gdkGL_gdkutils.h"

#include <gdk/gdkprivate.h>


GdkVisual *
gdk_colormap_get_visual(GdkColormap *gdk_colormap)
{
  g_return_val_if_fail(gdk_colormap != NULL, (GdkVisual *)NULL);

  return (((GdkColormapPrivate*) gdk_colormap)->visual);
}

gchar *
gdk_str_gdk_visual_type(GdkVisualType gdk_visual_type)
{
  switch (gdk_visual_type)
    {
    case GDK_VISUAL_STATIC_GRAY:
      return "GDK_VISUAL_STATIC_GRAY";
      break;
    case GDK_VISUAL_GRAYSCALE:
      return "GDK_VISUAL_GRAYSCALE";
      break;
    case GDK_VISUAL_STATIC_COLOR:
      return "GDK_VISUAL_STATIC_COLOR";
      break;
    case GDK_VISUAL_PSEUDO_COLOR:
      return "GDK_VISUAL_PSEUDO_COLOR";
      break;
    case GDK_VISUAL_TRUE_COLOR:
      return "GDK_VISUAL_TRUE_COLOR";
      break;
    case GDK_VISUAL_DIRECT_COLOR:
      return "GDK_VISUAL_DIRECT_COLOR";
      break;
    default:
      return (gchar *)NULL;
      break;
    }
}

gchar *
gdk_str_gdk_byte_order(GdkByteOrder byte_order)
{
  switch (byte_order)
    {
    case GDK_LSB_FIRST:
      return "GDK_LSB_FIRST";
      break;
    case GDK_MSB_FIRST:
      return "GDK_MSB_FIRST";
      break;
    default:
      return (gchar *)NULL;
      break;
    }
}
