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
#else
# include <cassert>
# define I(EXPR) assert(EXPR)
#endif

#define _(MSG) gettext(MSG)

using namespace std;

GtkWidget *
options_dialog::create_widget()
{
  GtkWidget *dialog = gtk_dialog_new();
  gtk_window_set_policy(GTK_WINDOW(dialog), FALSE, FALSE, FALSE);
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_signal_connect(GTK_OBJECT(dialog), "destroy",
		     GTK_SIGNAL_FUNC(remove_widget), this);
#if 0
  gtk_signal_connect(GTK_OBJECT(dialog), "delete_event",
		     GTK_SIGNAL_FUNC(handle_delete_event), this);
#endif

  populate(dialog);

  widgets.push_back(dialog);
  return dialog;
}

void
options_dialog::add_page(const char *tab_text, options_page *page)
{
  pages.push_back(make_pair(tab_text, page));
}

options_dialog::~options_dialog()
{
  for (vector<GtkWidget *>::iterator i = widgets.begin();
       i != widgets.end();
       ++i)
    gtk_signal_disconnect_by_data(GTK_OBJECT(*i), this);
}

void
options_dialog::handle_ok(GtkWidget *button,
			  gpointer data)
{
  gtk_main_quit();
}

void
options_dialog::handle_cancel(GtkWidget *button,
			      gpointer data)
{
  gtk_main_quit();
}

/* Handles a delete event on the dialog.  */
gint
options_dialog::handle_delete_event(GtkWidget *dialog,
				    GdkEventAny *event,
				    gpointer data)
{
  gtk_main_quit();
  return 1;
}

void
options_dialog::populate(GtkWidget *dialog)
{
  GtkWidget *notebook1 = gtk_notebook_new();
  for (vector<pair<string, options_page *> >::iterator i = pages.begin();
       i != pages.end();
       ++i)
    {
      GtkWidget *page_widget = i->second->create_widget();
      gtk_widget_show(page_widget);
      gtk_notebook_append_page(GTK_NOTEBOOK(notebook1), page_widget,
			       gtk_label_new(i->first.c_str()));
    }
  gtk_widget_show(notebook1);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), notebook1,
		     FALSE, FALSE, 0);

  /* Label for the OK button.  */
  const char *ok_text = _("OK");
  GtkWidget *ok_button = gtk_button_new_with_label(ok_text);
  gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
		     GTK_SIGNAL_FUNC(handle_ok), this);
  gtk_widget_show(ok_button);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), ok_button,
		     FALSE, FALSE, 0);

  /* Label for the Cancel button.  */
  const char *cancel_text = _("Cancel");
  GtkWidget *cancel_button = gtk_button_new_with_label(cancel_text);
  gtk_signal_connect(GTK_OBJECT(cancel_button), "clicked",
		     GTK_SIGNAL_FUNC(handle_cancel), this);
  gtk_widget_show(cancel_button);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), cancel_button,
		     FALSE, FALSE, 0);

  gtk_window_set_focus(GTK_WINDOW(dialog), ok_button);
}

void
options_dialog::remove_widget(GtkObject *object, gpointer data)
{
  GtkWidget *widget = GTK_WIDGET(object);
  options_dialog *d = static_cast<options_dialog *>(data);

  I(d != NULL);
  vector<GtkWidget *>::iterator k
    = find(d->widgets.begin(), d->widgets.end(), widget);
  if (k != d->widgets.end())
    d->widgets.erase(k);
}

