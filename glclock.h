/* -*-C++-*- */
/* rglclock - Rotating GL Clock.
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

class module;

class glclock
{
public:
  glclock ();
  ~glclock ();
public:
  operator GtkWidget *() const;
protected:
  static void create_context (GtkWidget *, gpointer);
  static gint handle_configure_event (GtkWidget *, GdkEventConfigure *, gpointer);
  static gint handle_expose_event (GtkWidget *, GdkEventExpose *, gpointer);
  static gint handle_button_event (GtkWidget *, GdkEventButton *, gpointer);
  static gint update (gpointer);
private:
  module *m;
  GtkWidget *drawing_area;
  GdkGLContext *context;
  time_t t;
  guint timeout_id;
  double rot_velocity;
  double rot_x, rot_y, rot_z;
  double press_x, press_y;
};

#endif /* not glclock_h */
