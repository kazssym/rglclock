/* -*-C++-*- */
/* Copyright (C) 1999 Hypercore Software Design, Ltd.

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

#ifndef AUTOWIDGET_H
#define AUTOWIDGET_H 1

#include <gtk/gtkwidget.h>

class auto_widget
{
private:
  GtkWidget *w;
public:
  auto_widget() throw()
    : w(0) {}
  auto_widget(GtkWidget *widget) throw()
    : w(widget)
    {
      if (w != 0)
	{gtk_widget_ref(w); gtk_object_sink(GTK_OBJECT(w));}
    }
  auto_widget(const auto_widget &x) throw()
    : w(x.w) {if (w != 0) gtk_widget_ref(w);}
  ~auto_widget() throw()
    {if (w != 0) gtk_widget_unref(w);}
public:
  auto_widget &operator=(const auto_widget &x) throw()
    {
      if (&x != this)
        {
	  if (w != 0) gtk_widget_unref(w);
	  w = x.w;
	  if (w != 0) gtk_widget_ref(w);
	}
      return *this;
    }
  GtkWidget &operator*() const throw()
    {return *w;}
  GtkWidget *operator->() const throw()
    {return w;}
public:
  GtkWidget *get() const throw()
    {return w;}
};

#endif /* not AUTOWIDGET_H */

