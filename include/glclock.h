/* -*-C++-*- */
/* rglclock - Rotating GL Clock.
   Copyright (C) 1998, 2000 Hypercore Software Design, Ltd.

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

#include "gl_context.h"
#include "utils.h"
#include <module.h>
#include <gtk/gtkwidget.h>
#include <gdk/gdktypes.h>
#include <time.h>

using rglclock::gl_context;

class glclock;

/* The options dialog.  */
class clock_options_dialog: public options_dialog
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

    /* Updates the state of subwidgets.  */
    void update(GtkWidget *page);

    /* Applies the current state.  */
    void apply(GtkWidget *page);
  };
private:
  general_options_page general;
public:
  explicit clock_options_dialog(glclock *);
};

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
  gl_context context;
  std::vector<GtkWidget *> widgets;

  /* `Options' dialog.  */
  clock_options_dialog options;

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

public:
  /* Shows the `Options' dialog.  */
  void show_options_dialog(GtkWindow *);
};

#endif /* not GLCLOCK_H */
