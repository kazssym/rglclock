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
   02111-1307, USA.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#undef const

#include "glclock.h"

#include <gtk/gtk.h>
#include <libintl.h>
#include <cstring>

#ifdef ENABLE_TRANSIENT_FOR_HINT
# include <gdk/gdkx.h>
#endif

#define _(MSG) gettext(MSG)

using namespace std;

void
options_dialog::show()
{
  gtk_widget_show(dialog);

  gtk_grab_add(dialog);
  gtk_main();
  gtk_grab_remove(dialog);

  gtk_widget_hide(dialog);
}

options_dialog::~options_dialog()
{
  gtk_widget_destroy(dialog);
}

options_dialog::options_dialog(GtkWidget *parent)
  : parent_widget(parent),
    dialog(NULL)
{
  dialog = gtk_dialog_new();
  gtk_window_set_policy(GTK_WINDOW(dialog), FALSE, FALSE, FALSE);
  gtk_window_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
  gtk_signal_connect_after(GTK_OBJECT(dialog), "realize",
			   GTK_SIGNAL_FUNC(finish_realize), this);
  gtk_signal_connect(GTK_OBJECT(dialog), "delete_event",
		     GTK_SIGNAL_FUNC(handle_delete_event), this);

  populate(dialog);
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

/* Finish the realize process.  */
void
options_dialog::finish_realize(GtkWidget *dialog,
			     gpointer data)
{
  options_dialog *options = static_cast <options_dialog *> (data);

#ifdef ENABLE_TRANSIENT_FOR_HINT
  if (options->parent_widget != NULL)
    {
      GdkWindow *w = dialog->window;
      XSetTransientForHint(GDK_WINDOW_XDISPLAY(w), GDK_WINDOW_XWINDOW(w),
			   GDK_WINDOW_XWINDOW(options->parent_widget->window));
    }
#endif /* ENABLE_TRANSIENT_FOR_HINT */
}

void
options_dialog::populate(GtkWidget *dialog)
{
  GtkWidget *notebook1 = gtk_notebook_new();
  {
    GtkWidget *general_page = gtk_vbox_new(FALSE, 0);
    {
      const char *text1 = _("Update frequency:");
      GtkWidget *label1 = gtk_label_new(text1);
      gtk_widget_show(label1);
      gtk_box_pack_start(GTK_BOX(general_page), label1,
			 FALSE, FALSE, 10);
    }
    gtk_widget_show(general_page);

    /* Label for the General tab.  */
    const char *general_tab_text = _("General");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook1), general_page,
			     gtk_label_new(general_tab_text));

    GtkWidget *rendering_page = gtk_vbox_new(FALSE, 0);
    {
    }
    gtk_widget_show(rendering_page);

    const char *rendering_tab_text = _("Rendering");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook1), rendering_page,
			     gtk_label_new(rendering_tab_text));
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

  gtk_window_set_default(GTK_WINDOW(dialog), ok_button);
}

