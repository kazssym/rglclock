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

#define _(MSG) gettext(MSG)

using namespace std;

GtkWidget *
clock_options_dialog::general_options_page::create_widget()
{
  GtkWidget *box = gtk_vbox_new(FALSE, 0);
  {
    const char *text1 = _("Update frequency:");
    GtkWidget *label1 = gtk_label_new(text1);
    gtk_widget_show(label1);
    gtk_box_pack_start(GTK_BOX(box), label1,
		       FALSE, FALSE, 10);
  }

  return box;
}

clock_options_dialog::clock_options_dialog()
{
  /* Label for the General page.  */
  const char *general_tab_text = _("General");
  add_page(general_tab_text, &general);
}

