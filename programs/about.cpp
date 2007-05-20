/*
 * RGLClock - 3D rotating clock
 * Copyright (C) 1998, 1999, 2000, 2002, 2007 Hypercore Software
 * Design, Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "about.h"

#include <cstdio>
#include <cassert>
#include <gtk/gtk.h>

#if ENABLE_NLS
#include <libintl.h>
#define _(t) gettext (t)
#else
#define _(t) (t)
#endif

about_dialog::about_dialog (GtkWindow *parent)
{
    this->initialize (parent);
}

about_dialog::about_dialog (const about_dialog &dialog)
{
    GtkWindow *parent = GTK_WINDOW (dialog.to_widget ())->transient_parent;
    this->initialize (parent);
}

about_dialog::~about_dialog ()
{
    if (dialog != NULL)
    {
        gtk_object_destroy (GTK_OBJECT (dialog));
    }
}

static void handle_ok_clicked (GtkButton *button, gpointer data) throw ();

void about_dialog::initialize (GtkWindow *parent)
{
    using std::snprintf;

    const int buf_size = 512; 
    gchar buf[buf_size];

    dialog = gtk_dialog_new ();
    gtk_object_ref (GTK_OBJECT (dialog));
    gtk_widget_set_usize (dialog, 540, 360);
    gtk_window_set_policy (GTK_WINDOW (dialog), false, false, false);
    gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (dialog), true);
    if (parent != NULL)
    {
        gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);
    }

    snprintf (buf, buf_size, _ ("About %s"), PACKAGE_NAME);
    gtk_window_set_title (GTK_WINDOW (dialog), buf);

    GtkWidget *hbox1 = gtk_hbox_new (false, 0);
    gtk_widget_show (hbox1);
    gtk_container_set_border_width (GTK_CONTAINER (hbox1), 0);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox1,
                        true, true, 0);

    GtkWidget *vbox1 = gtk_vbox_new (false, 10);
    gtk_widget_show (vbox1);
    gtk_container_set_border_width (GTK_CONTAINER (vbox1), 20);
    gtk_box_pack_start (GTK_BOX (hbox1), vbox1, true, true, 0);

    snprintf (buf, buf_size,
              _ ("%s version %s\n"
                 "Copyright (C) 1998, 1999, 2000, 2002, 2007 "
                 "Hypercore Software Design, Ltd."),
              PACKAGE_NAME, PACKAGE_VERSION);
    label1 = gtk_label_new (buf);
    gtk_widget_show (label1);
    gtk_widget_set_usize (label1, 500, 0);
    gtk_misc_set_alignment (GTK_MISC (label1), 0.0, 0.0);
    //gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
    gtk_label_set_line_wrap (GTK_LABEL (label1), true);
    gtk_box_pack_start (GTK_BOX (vbox1), label1, false, false, 0);

    label2 = gtk_label_new
        (_ ("This program is free software; you can redistribute it "
            "and/or modify it under the GNU General Public License "
            "as publishd by the Free Software Foundation; "
            "either version 2 of the License, or (at your option) "
            "any later version."));
    gtk_widget_show (label2);
    gtk_widget_set_usize (label2, 500, 0);
    gtk_misc_set_alignment (GTK_MISC (label2), 0.0, 0.0);
    //gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
    gtk_label_set_line_wrap (GTK_LABEL (label2), true);
    gtk_box_pack_start (GTK_BOX (vbox1), label2, false, false, 0);

    snprintf (buf, 256, _ ("Report bugs to <%s>."), PACKAGE_BUGREPORT);
    label3 = gtk_label_new (buf);
    gtk_widget_show (label3);
    gtk_widget_set_usize (label3, 500, 0);
    gtk_misc_set_alignment (GTK_MISC (label3), 0.0, 0.0);
    //gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
    gtk_label_set_line_wrap (GTK_LABEL (label3), true);
    gtk_box_pack_end (GTK_BOX (vbox1), label3, false, false, 0);

    ok_button = gtk_button_new_with_label (_ ("OK"));
    gtk_widget_show (ok_button);
    gtk_widget_set_usize (ok_button, 100, 0);
    gtk_window_set_focus (GTK_WINDOW (dialog), ok_button);
    gtk_signal_connect (GTK_OBJECT (ok_button), "clicked",
                        GTK_SIGNAL_FUNC (&handle_ok_clicked), this);
    gtk_box_pack_end (GTK_BOX (GTK_DIALOG (dialog)->action_area),
                      ok_button, false, false, 0);
}

void handle_ok_clicked (GtkButton *button, gpointer data) throw ()
{
    about_dialog *dialog = static_cast<about_dialog *> (data);
    gtk_widget_hide (dialog->to_widget ());
}

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-offsets: ((substatement-open . 0))
 * End:
 */