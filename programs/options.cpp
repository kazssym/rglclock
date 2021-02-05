// options.cpp
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
#include <config.h>
#endif

#include "utils.h"

#include <gtk/gtk.h>
#include <gettext.h>
#include <algorithm>
#include <cstring>
#include <cassert>

using namespace std;

#define _(String) gettext(String)
#define N_(String) gettext_noop(String)

void
options_dialog::handle_ok(GtkWidget *button)
{
    GtkWidget *dialog = gtk_widget_get_ancestor(button, gtk_dialog_get_type());
    assert(GTK_IS_DIALOG(dialog));

    GList *dialog_children
        = gtk_container_get_children(
            GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))));
    assert(dialog_children != NULL);

    GtkWidget *notebook = GTK_WIDGET(dialog_children->data);
    assert(GTK_IS_NOTEBOOK(notebook));

    int j = 0;
    for (vector<pair<string, options_page *> >::iterator i = pages.begin();
        i != pages.end();
        ++i) {
        GtkWidget *page_widget
            = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), j++);
        assert(page_widget != NULL);
        i->second->apply(page_widget);
    }

    close(positive_action());
}

void
options_dialog::handle_cancel(GtkWidget *button)
{
    close(negative_action());
}

void
options_dialog::update(GtkDialog *widget)
{
    GList *children = gtk_container_get_children(
        GTK_CONTAINER(gtk_dialog_get_content_area(widget)));
    assert(children != NULL);

    /* This assumes the first child is a GtkNotebook.  */
    GtkWidget *notebook = GTK_WIDGET(children->data);
    assert(GTK_IS_NOTEBOOK(notebook));

    int j = 0;
    for (vector<pair<string, options_page *> >::iterator i = pages.begin();
        i != pages.end(); ++i) {
        GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), j++);
        assert(page != NULL);

        i->second->update(page);
    }
}

namespace
{
    /* Delivers an OK to the options dialog.  */
    void
    deliver_ok(GtkWidget *w, gpointer data) throw ()
    {
        options_dialog *recipient = static_cast<options_dialog *>(data);
        assert(recipient != NULL);

        recipient->handle_ok(w);
    }

    /* Delivers a cancel to the options dialog.  */
    void
    deliver_cancel(GtkWidget *w, gpointer data) throw ()
    {
        options_dialog *recipient = static_cast<options_dialog *>(data);
        assert(recipient != NULL);

        recipient->handle_cancel(w);
    }
} // (unnamed)

void
options_dialog::populate(GtkDialog *dialog)
{
    GtkWidget *notebook1 = gtk_notebook_new();
    for (vector<pair<string, options_page *> >::iterator i = pages.begin();
        i != pages.end();
        ++i) {
        GtkWidget *page_widget = i->second->create_widget();
        assert(page_widget != NULL);
        gtk_widget_show(page_widget);
        gtk_notebook_append_page(GTK_NOTEBOOK(notebook1), page_widget,
                                                            gtk_label_new(i->first.c_str()));
    }
    gtk_widget_show(notebook1);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(dialog)), notebook1,
        FALSE, FALSE, 0);

    auto &&ok_button = gtk_button_new_with_label(_("OK"));
    assert(GTK_IS_BUTTON(ok_button));
    g_signal_connect(G_OBJECT(ok_button), "clicked",
        G_CALLBACK(&deliver_ok), this);
    gtk_widget_show(ok_button);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_action_area(dialog)), ok_button,
        FALSE, FALSE, 0);

    auto &&cancel_button = gtk_button_new_with_label(_("Cancel"));
    assert(GTK_IS_BUTTON(cancel_button));
    g_signal_connect(G_OBJECT(cancel_button), "clicked",
        G_CALLBACK(&deliver_cancel), this);
    gtk_widget_show(cancel_button);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_action_area(dialog)), cancel_button,
        FALSE, FALSE, 0);

    gtk_window_set_focus(GTK_WINDOW(dialog), ok_button);
}

void
options_dialog::configure(GtkDialog *widget)
{
    gtk_window_set_title(GTK_WINDOW(widget), _("Options"));
    gtk_window_set_resizable(GTK_WINDOW(widget), false);
    gtk_window_set_position(GTK_WINDOW(widget), GTK_WIN_POS_CENTER);

    populate(widget);
}

void
options_dialog::add_page(const char *tab_text, options_page *page)
{
    pages.push_back(make_pair(tab_text, page));
}
