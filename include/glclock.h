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

#include "utils.h"
#include "gdkgl.h"
#include <module.h>
#include <gtk/gtkwidget.h>
#include <gdk/gdktypes.h>
#include <time.h>

/* Clock application.  */
class glclock
{
public:
  /* Call-back interface that is used to implement hooks about
     options.  */
  struct options_callback
  {
    virtual void options_changed(glclock *) = 0;
  };

public:
  /* Returns the best visual for this class.  */
  static GdkVisual *best_visual();

protected:
  static void remove_widget(GtkObject *, gpointer);
  static gint handle_expose_event (GtkWidget *, GdkEventExpose *, gpointer);
  static gint handle_button_event (GtkWidget *, GdkEventButton *, gpointer);
  static gint update (gpointer);
private:
  module *m;
  std::vector<options_callback *> callbacks;
  int timeout_rate;
  guint timeout_id;
  GdkGLContext *context;
  std::vector<GtkWidget *> widgets;
  double rot_velocity;
  double rot_x, rot_y, rot_z;
  double press_x, press_y;

public:
  /* Constructs this clock with default properties.  */
  glclock();

  /* Destructs this clock.  */
  ~glclock();

public:
  void add_callback(options_callback *);
  void remove_callback(options_callback *);
  int update_rate() const
    {return timeout_rate;}
  void set_update_rate(int);

  /* Sets modules's property NAME to VALUE.  */
  void set_module_prop(const std::string &name, const std::string &value)
    {m->set_prop(name, value);}

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
};

/* The about dialog.  */
class about_dialog
  : public modal_dialog
{
protected:
  static void handle_ok(GtkWidget *, gpointer);

protected:
  /* Configures a dialog widget.  */
  void configure(GtkDialog *widget);

protected:
  GtkWidget *create_widget();

  /* Populates a dialog with subwidgets.  */
  void populate(GtkDialog *widget);
};

#endif /* not GLCLOCK_H */
