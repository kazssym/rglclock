/* rglclock - Rotating GL Clock.
   Copyright (C) 1998 Hypercore Software Design, Ltd.

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

#include <gtk/gtk.h>

static
void ok(GtkWidget *w)
{
  gtk_main_quit();
}

void
show_about(GtkWidget *parent)
{
  GtkWidget *dialog = gtk_dialog_new();
  gtk_window_set_policy(GTK_WINDOW(dialog), FALSE, FALSE, FALSE);
  GtkWidget *child;

  child = gtk_label_new(PACKAGE " " VERSION "\n"
			"Copyright (C) 1998, 1999 Hypercore Software Design, Ltd.");
  gtk_label_set_justify(GTK_LABEL(child), GTK_JUSTIFY_LEFT);
  gtk_widget_show(child);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), child, FALSE, FALSE, 0);

  child =  gtk_button_new_with_label("OK");
  gtk_signal_connect(GTK_OBJECT(child), "clicked", GTK_SIGNAL_FUNC(ok), NULL);
  gtk_widget_show(child);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), child, TRUE, FALSE, 0);

  gtk_widget_show(dialog);

  gtk_widget_set_sensitive(parent, FALSE);
  gtk_main();
  gtk_widget_set_sensitive(parent, TRUE);

  gtk_widget_destroy(dialog);
}

