/* -*-C++-*- */
/* rglclock - R'ing GL Clock.
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#ifndef glclock_h
#define glclock_h 1

#include <time.h>
#include <gtk/gtkwidget.h>
#include <gdk/gdktypes.h>
#include "gdkGL/gdkGLtypes.h"

class glclock
{
public:
  glclock ();
  ~glclock ();
public:
  operator GtkWidget *() const;
protected:
  static void create_context (GtkWidget *, gpointer);
  static gint handle_expose_event (GtkWidget *, GdkEventExpose *, gpointer);
  static void update (gpointer);
private:
  GtkWidget *drawing_area;
  GdkGLContext *context;
  struct tm tm;
  guint timeout_id;
};

#endif /* not glclock_h */
