/* -*-C++-*- */
/* rglclock - Rotating GL Clock.
   Copyright (C) 1998, 1999 Hypercore Software Design, Ltd.

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
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.  */

#ifndef GLCLOCK_H
#define GLCLOCK_H 1

#include "gdkgl.h"
#include <gtk/gtkmenufactory.h>
#include <gtk/gtkwidget.h>
#include <gdk/gdktypes.h>
#include <time.h>

class module;

class glclock
{
public:
  glclock ();
  ~glclock ();
public:
  operator GtkWidget *() const;
  void describe (GtkWidget *);
protected:
  struct menu_listener
  {
    glclock *target;
    void (glclock::*activate) (GtkWidget *);
    static void menu_activate (GtkWidget *, gpointer);
  };
protected:
  static gint handle_configure_event (GtkWidget *, GdkEventConfigure *, gpointer);
  static gint handle_destroy_event (GtkWidget *, GdkEventAny *, gpointer);
  static gint handle_expose_event (GtkWidget *, GdkEventExpose *, gpointer);
  static gint handle_button_event (GtkWidget *, GdkEventButton *, gpointer);
  static gint update (gpointer);
  static void menu_quit (GtkWidget *, gpointer);
private:
  module *m;
  GtkWidget *drawing_area;
  GtkMenuFactory *menu_factory;
  guint timeout_id;
  GdkGLContext *context;
  double rot_velocity;
  double rot_x, rot_y, rot_z;
  time_t t;
  double press_x, press_y;
  menu_listener about;
};

class about_dialog
{
private:
  GtkWidget *dialog;
protected:
  static void populate(GtkWidget *);
public:
  about_dialog();
  ~about_dialog();
public:
  void show(GtkWidget *);
};

#endif /* not GLCLOCK_H */
