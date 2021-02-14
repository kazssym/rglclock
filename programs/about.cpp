// about.cpp
// Copyright (C) 1998-2007 Hypercore Software Design, Ltd.
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "about.h"

#include <gtk/gtk.h>
#include <gettext.h>
#include <cstdio>
#include <cassert>

using std::snprintf;

#define _(String) gettext(String)
#define N_(String) gettext_noop(String)


about_dialog::about_dialog(GtkWindow *parent)
{
    gtk_window_set_resizable(GTK_WINDOW(&*_widget), false);
    if (parent != nullptr) {
        gtk_window_set_transient_for(GTK_WINDOW(&*_widget), parent);
    }

    char title[128];
    snprintf(title, 128, _("About %s"), PACKAGE_NAME);
    gtk_window_set_title(GTK_WINDOW(&*_widget), title);

    auto &&content_area = gtk_dialog_get_content_area(GTK_DIALOG(&*_widget));
    gtk_widget_set_margin_start(content_area, 12);
    gtk_widget_set_margin_end(content_area, 12);
    gtk_widget_set_margin_top(content_area, 12);
    gtk_widget_set_margin_bottom(content_area, 12);

    char label1_text[512];
    snprintf(label1_text, 512,
        "%s version %s\n"
        "Copyright %s 1998-2007 Hypercore Software Design, Ltd.\n"
        "Copyright %s 2021 Kaz Nishimura",
        PACKAGE_NAME, PACKAGE_VERSION, _("(C)"), _("(C)"));

    auto &&label1 = gtk_label_new(label1_text);
    gtk_label_set_max_width_chars(GTK_LABEL(label1), 60);
    gtk_label_set_line_wrap(GTK_LABEL(label1), true);
    gtk_label_set_xalign(GTK_LABEL(label1), 0.0F);
    gtk_widget_show(label1);
    gtk_box_pack_start(GTK_BOX(content_area), label1, false, false, 4);

    auto &&label2 = gtk_label_new(_(
        "This program is free software; you can redistribute it and/or modify "
        "it under the GNU General Public License as publishd by the Free "
        "Software Foundation; either version 3 of the License, or (at your "
        "option) any later version."));
    gtk_label_set_max_width_chars(GTK_LABEL(label2), 60);
    gtk_label_set_line_wrap(GTK_LABEL(label2), true);
    gtk_label_set_xalign(GTK_LABEL(label2), 0.0F);
    gtk_widget_show(label2);
    gtk_box_pack_start(GTK_BOX(content_area), label2, false, false, 4);

    char label3_text[128];
    snprintf(label3_text, 128, _("Report bugs to <%s>."), PACKAGE_BUGREPORT);

    auto &&label3 = gtk_label_new(label3_text);
    gtk_label_set_max_width_chars(GTK_LABEL(label3), 60);
    gtk_label_set_line_wrap(GTK_LABEL(label3), true);
    gtk_label_set_xalign(GTK_LABEL(label3), 0.0F);
    gtk_widget_show(label3);
    gtk_box_pack_end(GTK_BOX(content_area), label3, false, false, 4);

    gtk_dialog_add_button(GTK_DIALOG(&*_widget), _("OK"), GTK_RESPONSE_OK);
}

int about_dialog::show_modal() const
{
    return gtk_dialog_run(GTK_DIALOG(&*_widget));
}
