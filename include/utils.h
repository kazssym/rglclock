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

#ifndef UTILS_H
#define UTILS_H 1

#include <gtk/gtkdialog.h>
#include <vector>
#include <string>

class controller
{
protected:
  static controller *to_ptr(gpointer data)
    {return static_cast<controller *>(data);}
private:
  static void remove_widget(GtkObject *, gpointer) throw ();
private:
  std::vector<GtkWidget *> widgets;
public:
  virtual ~controller();
public:
  gpointer func_data()
    {return this;}
protected:
  void add(GtkWidget *);
};

/* Base component for modal dialogs.  */
class modal_dialog
{
public:
  /* Callback interface for actions.  */
  struct action_callback
  {
    virtual void action_taken(int key) = 0;
  };

public:
  /* Common key for negative actions.  */
  static int negative_action() {return 0;}

  /* Common key for positive actions.  */
  static int positive_action() {return 1;}

private:
  /* Callbacks to notify actions taken.  */
  vector<action_callback *> action_callbacks;

  /* Widget created for this dialog.  */
  GtkWidget *window;

public:
  /* Constructs this dialog.  */
  modal_dialog();

  /* Destructs this dialog.  */
  virtual ~modal_dialog();

public:
  /* Adds an action callback.  */
  void add(action_callback *callback);

  /* Activates this dialog.  */
  void act(GtkWindow *parent);

  /* Handles a delete event on the widget.  */
  bool handle_delete_event(GtkWidget *, GdkEventAny *);

protected:
  /* Takes an action.  */
  void take_action(int key);

  /* Deactivates this dialog.  */
  void close(int key);

protected:
  /* Configures a dialog.  */
  virtual void configure(GtkDialog *) = 0;

  /* Updates subwidgets.  */
  virtual void update(GtkDialog *) {}

protected:
  /* Creates a window if not.  */
  void create_window();

  /* Destroys the window of this dialog.  */
  void destroy_window();
};

struct options_page
{
  virtual GtkWidget *create_widget() = 0;
  virtual void update(GtkWidget *) {} // = 0;
  virtual void apply(GtkWidget *) = 0;
};

/* The options dialog.  */
class options_dialog
  : public modal_dialog
{
private:
  std::vector<std::pair<std::string, options_page *> > pages;

public:
  void add_page(const char *, options_page *);

  /* Handles an OK.  */
  void handle_ok(GtkWidget *);

  /* Handles a cancel.  */
  void handle_cancel(GtkWidget *);

protected:
  /* Configures a dialog widget.  */
  void configure(GtkDialog *widget);

  /* Updates the state of subwidgets.  */
  void update(GtkDialog *widget);

protected:
  /* Populates a dialog widgets with subwidgets.  */
  void populate(GtkDialog *widget);
};

#endif /* not UTILS_H */
