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
#ifndef __GDK_GL_UTILS_H__
#define __GDK_GL_UTILS_H__


#include <gdk/gdk.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


gchar *gdk_str_gdk_visual_type (GdkVisualType  visual_type);
gchar *gdk_str_gdk_byte_order  (GdkByteOrder   byte_order);

GdkVisual* gdk_colormap_get_visual (GdkColormap *colormap);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GDK_GL_UTILS_H__ */
