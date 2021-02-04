// modaldialog.cpp
// Copyright (C) 1999 Hypercore Software Design, Ltd.
// Copyright (C) 2021 Kaz Nishimura
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#undef const

#include "utils.h"

#include <gtk/gtk.h>
//#include <algorithm>
//#include <cstring>

#ifdef HAVE_NANA_H
# include <nana.h>
# include <cstdio>
#else
# include <cassert>
# define I assert
#endif

using namespace std;

void
modal_dialog::take_action(int key)
{
  for (vector<action_callback *>::iterator i = action_callbacks.begin();
       i != action_callbacks.end();
       ++i)
    (*i)->action_taken(key);
}

void
modal_dialog::close(int key)
{
  gtk_widget_hide(window);
  take_action(key);
}

void
modal_dialog::add(action_callback *callback)
{
  action_callbacks.push_back(callback);
}

namespace
{
  /* Delivers a delete event to the dialog's handler.  */
  gint
  deliver_delete_event(GtkWidget *w, GdkEventAny *e, gpointer data) throw ()
  {
    modal_dialog *recipient = static_cast<modal_dialog *>(data);
    I(recipient != NULL);

    return recipient->handle_delete_event(w, e);
  }
} // (unnamed)

void
modal_dialog::create_window()
{
  if (window == NULL)
    {
      window = gtk_dialog_new();
#ifdef L
      L("modal_dialog[%p]: Created a GtkDialog %p\n", this, window);
#endif

      gtk_window_set_modal(GTK_WINDOW(window), true);
      g_signal_connect(G_OBJECT(window), "delete_event",
			 G_CALLBACK(&deliver_delete_event), this);

      configure(GTK_DIALOG(window));
    }
}

void
modal_dialog::destroy_window()
{
  if (window != NULL)
    {
      gtk_widget_destroy(window);
      window = NULL;
    }
}

void
modal_dialog::act(GtkWindow *parent)
{
  create_window();

  if (!gtk_widget_get_visible(window))
    {
      gtk_window_set_transient_for(GTK_WINDOW(window), parent);

      update(GTK_DIALOG(window));

      gtk_widget_show(window);
    }
}

/* Handles a delete event on the dialog.  */
bool
modal_dialog::handle_delete_event(GtkWidget *dialog,
				  GdkEventAny *event)
{
  close(negative_action());
  return true;
}

modal_dialog::~modal_dialog()
{
  destroy_window();
}

modal_dialog::modal_dialog()
  : window(NULL)
{
}

