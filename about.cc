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

#ifndef DISABLE_TRANSIENT_FOR_HINT
# define ENABLE_TRANSIENT_FOR_HINT 1
#endif

#ifdef ENABLE_TRANSIENT_FOR_HINT
# include <gdk/gdkx.h>
#endif

/* Shows this about dialog and wait.  PARENT will become insensible
   while this dialog is shown.  */
void
about_dialog::show(GtkWidget *parent)
{
#ifdef ENABLE_TRANSIENT_FOR_HINT
  guint h = 0;
  if (parent != NULL)
    h = gtk_signal_connect(GTK_OBJECT(dialog), "configure_event",
			   GTK_SIGNAL_FUNC(handle_configure_event), parent);
#endif /* ENABLE_TRANSIENT_FOR_HINT */

  gtk_widget_show(dialog);

  gtk_widget_set_sensitive(parent, FALSE);
  gtk_main();
  gtk_widget_set_sensitive(parent, TRUE);

  gtk_widget_hide(dialog);
#ifdef ENABLE_TRANSIENT_FOR_HINT
  if (parent != NULL)
    gtk_signal_disconnect(GTK_OBJECT(dialog), h);
#endif /* ENABLE_TRANSIENT_FOR_HINT */
}

about_dialog::~about_dialog()
{
  gtk_widget_destroy(dialog);
}

about_dialog::about_dialog()
  : dialog(NULL)
{
  dialog = gtk_dialog_new();
  gtk_window_set_policy(GTK_WINDOW(dialog), FALSE, FALSE, FALSE);
  gtk_signal_connect(GTK_OBJECT(dialog), "delete_event",
		     GTK_SIGNAL_FUNC(handle_delete_event), this);

  populate(dialog);
}

/* Populates a dialog with children.  */
void
about_dialog::populate(GtkWidget *dialog)
{
  GtkWidget *child;

  /* Makes the vbox area.  */
  child = gtk_label_new(PACKAGE " " VERSION "\n"
			"Copyright (C) 1998, 1999 Hypercore Software Design, Ltd.");
  gtk_label_set_justify(GTK_LABEL(child), GTK_JUSTIFY_LEFT);
  gtk_misc_set_padding(GTK_MISC(child), 10, 10);
  gtk_widget_show(child);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), child,
		     FALSE, FALSE, 0);

  /* Makes the action area.  */
  child =  gtk_button_new_with_label("OK");
  gtk_signal_connect(GTK_OBJECT(child), "clicked",
		     GTK_SIGNAL_FUNC(handle_ok), dialog);
  gtk_widget_show(child);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), child,
		     TRUE, FALSE, 0);
}

/* Handles clicks on the OK button.  */
void
about_dialog::handle_ok(GtkWidget *dialog,
			gpointer data)
{
  gtk_main_quit();
}

/* Handles delete events on the dialog.  */
gint
about_dialog::handle_delete_event(GtkWidget *dialog,
				  GdkEventAny *event,
				  gpointer data)
{
  gtk_main_quit();
  return 1;
}

gint
about_dialog::handle_configure_event(GtkWidget *dialog,
				     GdkEventConfigure *event,
				     gpointer data)
{
  GtkWidget *par = static_cast <GtkWidget *> (data);

#ifdef ENABLE_TRANSIENT_FOR_HINT
  if (par != NULL)
    {
      GdkWindow *w = dialog->window;
      XSetTransientForHint(GDK_WINDOW_XDISPLAY(w), GDK_WINDOW_XWINDOW(w),
			   GDK_WINDOW_XWINDOW(par->window));
    }
#endif /* ENABLE_TRANSIENT_FOR_HINT */

  return 0;
}

