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

#include "glclock.h"

#include <gtk/gtk.h>
#include <libintl.h>
#include <cstdlib>

#ifdef HAVE_NANA_H
# include <nana.h>
# include <cstdio>
#else
# include <assert.h>
# define I assert
#endif

#define _(MSG) gettext(MSG)

using namespace std;

void
clock_options_dialog::general_options_page::apply(GtkWidget *widget)
{
  GtkWidget *update_input
    = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(widget), "update_input"));
  I(update_input != NULL);
  I(GTK_IS_ENTRY(update_input));
  int rate = atoi(gtk_entry_get_text(GTK_ENTRY(update_input)));
#ifdef L
  L("new update rate = %d\n", rate);
#endif
  I(target != NULL);
  target->set_update_rate(rate);
}

GtkWidget *
clock_options_dialog::general_options_page::create_widget()
{
  I(target != NULL);
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
  I(GTK_IS_VBOX(vbox));
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
  {
    GtkWidget *hbox1 = gtk_hbox_new(false, 0);
    {
      GtkWidget *label1 = gtk_label_new(_("Update rate: "));
      gtk_widget_show(label1);
      gtk_box_pack_start(GTK_BOX(hbox1), label1, false, false, 0);

      int rate = target->update_rate();
      GtkObject *update_adjust = gtk_adjustment_new(rate, 5, 30, 1, 5, 1);
      GtkWidget *update_input
	= gtk_spin_button_new(GTK_ADJUSTMENT(update_adjust), 1, 0);
      gtk_object_set_data(GTK_OBJECT(vbox), "update_input", update_input);
      gtk_widget_show(update_input);
      gtk_box_pack_start(GTK_BOX(hbox1), update_input, false, false, 0);

      /* Unit name for update rate */
      GtkWidget *unit_label = gtk_label_new(_("Hz"));
      gtk_widget_show(unit_label);
      gtk_box_pack_start(GTK_BOX(hbox1), unit_label, false, false, 0);
    }
    gtk_widget_show(hbox1);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, false, false, 0);

    GtkWidget *hbox2 = gtk_hbox_new(false, 0);
    {
      GtkWidget *check1
	= gtk_check_button_new_with_label(_("Map texture file: "));
      gtk_widget_show(check1);
      gtk_box_pack_start(GTK_BOX(hbox2), check1, false, false, 0);

      GtkWidget *entry1 = gtk_entry_new();
      gtk_widget_show(entry1);
      gtk_box_pack_start(GTK_BOX(hbox2), entry1, true, true, 0);

      GtkWidget *button1 = gtk_button_new_with_label(_("Select..."));
      gtk_widget_show(button1);
      gtk_box_pack_start(GTK_BOX(hbox2), button1, false, false, 0);      
    }
    gtk_widget_show(hbox2);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, false, false, 0);
  }

  return vbox;
}

clock_options_dialog::general_options_page::general_options_page(glclock *t)
  : target(t)
{
}

clock_options_dialog::clock_options_dialog(glclock *t)
  : general(t)
{
  /* Label for the General page.  */
  const char *general_tab_text = _("General");
  add_page(general_tab_text, &general);
}

