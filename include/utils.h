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

#include <gtk/gtkwindow.h>
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

class modal_dialog
  : public controller
{
protected:
  static gint handle_delete_event(GtkWidget *, GdkEventAny *,
				  gpointer) throw ();
public:
  void act(GtkWindow *parent);
protected:
  virtual GtkWidget *create_widget() = 0;
  void quit();
};

struct options_page
{
  virtual GtkWidget *create_widget() = 0;
  virtual void apply(GtkWidget *) = 0;
};

/* The options dialog.  */
class options_dialog
  : public modal_dialog
{
protected:
  static void handle_ok(GtkWidget *, gpointer) throw ();
  static void handle_cancel(GtkWidget *, gpointer) throw ();
private:
  std::vector<std::pair<std::string, options_page *> > pages;
public:
  void add_page(const char *, options_page *);
protected:
  void populate(GtkWidget *);
  GtkWidget *create_widget();
};

#endif /* not UTILS_H */
