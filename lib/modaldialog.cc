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

#include "utils.h"

#include <gtk/gtk.h>
#include <libintl.h>
//#include <algorithm>
//#include <cstring>

#ifdef HAVE_NANA_H
# include <nana.h>
# include <cstdio>
#else
# include <cassert>
# define I assert
#endif

#define _(MSG) gettext(MSG)

using namespace std;

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
modal_dialog::act(GtkWindow *parent)
{
  if (widget == NULL)
    {
      widget = create_widget();
      I(GTK_IS_WINDOW(widget));

      gtk_window_set_modal(GTK_WINDOW(widget), true);
      gtk_signal_connect(GTK_OBJECT(widget), "delete_event",
			 GTK_SIGNAL_FUNC(&deliver_delete_event), this);
    }

  gtk_window_set_transient_for(GTK_WINDOW(widget), parent);
  gtk_widget_show(widget);
}

void
modal_dialog::quit()
{
  gtk_widget_hide(widget);
}

/* Handles a delete event on the dialog.  */
bool
modal_dialog::handle_delete_event(GtkWidget *dialog,
				  GdkEventAny *event)
{
  quit();
  return true;
}

modal_dialog::~modal_dialog()
{
  if (widget != NULL)
    gtk_widget_destroy(widget);
}

modal_dialog::modal_dialog()
  : widget(NULL)
{
}

