/* rglclock - Rotating GL Clock.
   Copyright (C) 1999 Hypercore Software Design, Ltd.

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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#undef const

#include "options.h"

#include <gtk/gtk.h>
#include <libintl.h>
#include <algorithm>
#include <cstring>

#ifdef HAVE_NANA_H
# include <nana.h>
# include <cstdio>
#else
# include <cassert>
# define I assert
#endif

#define _(MSG) gettext(MSG)

using namespace std;

/* FIXME: Move these controller functions to another file for
   modularity.  */

void
controller::add(GtkWidget *widget)
{
  gtk_signal_connect(GTK_OBJECT(widget), "destroy",
		     GTK_SIGNAL_FUNC(remove_widget), func_data());
  widgets.push_back(widget);
}

controller::~controller()
{
  for (vector<GtkWidget *>::iterator i = widgets.begin();
       i != widgets.end();
       ++i)
    {
      gtk_signal_disconnect_by_data(GTK_OBJECT(*i), func_data());
      gtk_widget_set_sensitive(*i, false);
    }
}

void
controller::remove_widget(GtkObject *object, gpointer data) throw ()
{
  GtkWidget *widget = GTK_WIDGET(object);
  controller *c = to_ptr(data);

  I(c != NULL);
  vector<GtkWidget *>::iterator k
    = find(c->widgets.begin(), c->widgets.end(), widget);
  if (k != c->widgets.end())
    c->widgets.erase(k);
}

void
modal_dialog::act(GtkWindow *parent)
{
  GtkWidget *widget = create_widget();
  I(GTK_IS_WINDOW(widget));

  if (parent != NULL)
    gtk_window_set_transient_for(GTK_WINDOW(widget), parent);
  gtk_window_set_modal(GTK_WINDOW(widget), true);
  gtk_signal_connect(GTK_OBJECT(widget), "delete_event",
		     GTK_SIGNAL_FUNC(handle_delete_event), func_data());

  gtk_widget_show(widget);
  gtk_main();
  gtk_widget_destroy(widget);
}

void
modal_dialog::quit()
{
  gtk_main_quit();
}

/* Handles a delete event on the dialog.  */
gint
modal_dialog::handle_delete_event(GtkWidget *dialog,
				  GdkEventAny *event,
				  gpointer data) throw ()
{
  options_dialog *d = static_cast<options_dialog *>(to_ptr(data));
  I(d != NULL);

  d->quit();
  return 1;
}

void
options_dialog::populate(GtkWidget *dialog)
{
  I(GTK_IS_DIALOG(dialog));

  GtkWidget *notebook1 = gtk_notebook_new();
  for (vector<pair<string, options_page *> >::iterator i = pages.begin();
       i != pages.end();
       ++i)
    {
      GtkWidget *page_widget = i->second->create_widget();
      I(page_widget != NULL);
      gtk_widget_show(page_widget);
      gtk_notebook_append_page(GTK_NOTEBOOK(notebook1), page_widget,
			       gtk_label_new(i->first.c_str()));
    }
  gtk_widget_show(notebook1);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), notebook1,
		     FALSE, FALSE, 0);

  /* Label for the OK button.  */
  const char *ok_text = _("OK");
  I(ok_text != NULL);
  GtkWidget *ok_button = gtk_button_new_with_label(ok_text);
  I(GTK_IS_BUTTON(ok_button));
  gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
		     GTK_SIGNAL_FUNC(handle_ok), func_data());
  gtk_widget_show(ok_button);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), ok_button,
		     FALSE, FALSE, 0);

  /* Label for the Cancel button.  */
  const char *cancel_text = _("Cancel");
  I(cancel_text != NULL);
  GtkWidget *cancel_button = gtk_button_new_with_label(cancel_text);
  I(GTK_IS_BUTTON(cancel_button));
  gtk_signal_connect(GTK_OBJECT(cancel_button), "clicked",
		     GTK_SIGNAL_FUNC(handle_cancel), func_data());
  gtk_widget_show(cancel_button);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), cancel_button,
		     FALSE, FALSE, 0);

  gtk_window_set_focus(GTK_WINDOW(dialog), ok_button);
}

GtkWidget *
options_dialog::create_widget()
{
  GtkWidget *dialog = gtk_dialog_new();
  I(GTK_IS_DIALOG(dialog));
  gtk_window_set_policy(GTK_WINDOW(dialog), FALSE, FALSE, FALSE);
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

  populate(dialog);

#ifdef L
  L("options_dialog: Creating a widget %p\n", dialog);
#endif
  add(dialog);
  return dialog;
}

void
options_dialog::add_page(const char *tab_text, options_page *page)
{
#ifdef L
  L("options_dialog: Adding page %s -> %p\n", tab_text, page);
#endif
  pages.push_back(make_pair(tab_text, page));
}

void
options_dialog::handle_ok(GtkWidget *button,
			  gpointer data) throw ()
{
  options_dialog *d = static_cast<options_dialog *>(to_ptr(data));
  I(d != NULL);

  GtkWidget *dialog = gtk_widget_get_ancestor(button, gtk_dialog_get_type());
  I(GTK_IS_DIALOG(dialog));
  GList *dialog_children
    = gtk_container_children(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox));
  I(dialog_children != NULL);
  GtkWidget *notebook = GTK_WIDGET(dialog_children->data);
  I(GTK_IS_NOTEBOOK(notebook));
  int j = 0;
  for (vector<pair<string, options_page *> >::iterator i = d->pages.begin();
       i != d->pages.end();
       ++i)
    {
      GtkWidget *page_widget
	= gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), j++);
      I(page_widget != NULL);
      i->second->apply(page_widget);
    }

  d->quit();
}

void
options_dialog::handle_cancel(GtkWidget *button,
			      gpointer data) throw ()
{
  options_dialog *d = static_cast<options_dialog *>(to_ptr(data));
  I(d != NULL);

  d->quit();
}

