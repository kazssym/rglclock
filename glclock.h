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
   02111-1307, USA.

   As a special exception, you may copy and distribute this program
   linked with an OpenGL library in executable form without the source
   code for the library, only if such distribution is not prohibited
   by the library's license.  */

#ifndef GLCLOCK_H
#define GLCLOCK_H 1

#include "options.h"
#include "gdkgl.h"
#include <gtk/gtkitemfactory.h>
#include <gtk/gtkwidget.h>
#include <gdk/gdktypes.h>
#include <time.h>

class module;

/* Clock application.  */
class glclock
{
public:
  glclock ();
  ~glclock ();
protected:
  static void remove_widget(GtkObject *, gpointer);
  static gint handle_expose_event (GtkWidget *, GdkEventExpose *, gpointer);
  static gint handle_button_event (GtkWidget *, GdkEventButton *, gpointer);
  static gint update (gpointer);
  static void edit_options(gpointer, guint, GtkWidget *);
  static void menu_about(gpointer, guint, GtkWidget *);
  static void menu_quit(gpointer, guint, GtkWidget *);
private:
  module *m;
  GtkItemFactory *menu_factory;
  int timeout_rate;
  guint timeout_id;
  GdkGLContext *context;
  std::vector<GtkWidget *> widgets;
  double rot_velocity;
  double rot_x, rot_y, rot_z;
  double press_x, press_y;
  mutable GtkWidget *menu_parent;
public:
  int update_rate() const
    {return timeout_rate;}
  void set_update_rate(int);
  GtkWidget *create_widget();
};

/* The options dialog.  */
class clock_options_dialog
  : public options_dialog
{
public:
  class general_options_page
    : public options_page
  {
  private:
    glclock *target;
  public:
    explicit general_options_page(glclock *);
  public:
    GtkWidget *create_widget();
    void apply(GtkWidget *);
  };
private:
  general_options_page general;
public:
  explicit clock_options_dialog(glclock *);
public:
  void show();
protected:
  void populate(GtkWidget *);
};

/* The about dialog.  */
class about_dialog
{
private:
  GtkWidget *parent_widget;
  GtkWidget *dialog;
protected:
  static void populate(GtkWidget *);
  static void finish_realize(GtkWidget *, gpointer);
  static gint handle_delete_event(GtkWidget *, GdkEventAny *, gpointer);
  static void handle_ok(GtkWidget *, gpointer);
public:
  explicit about_dialog(GtkWidget *parent);
  ~about_dialog();
public:
  void show();
};

#endif /* not GLCLOCK_H */
