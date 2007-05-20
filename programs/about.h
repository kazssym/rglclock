/* -*- C++ -*-
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

#ifndef aboutH
#define aboutH 1

#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

class about_dialog
{
private:
    GtkWidget *dialog;
    GtkWidget *label1;
    GtkWidget *label2;
    GtkWidget *label3;
    GtkWidget *ok_button;

public:
    explicit about_dialog (GtkWindow *parent);
    about_dialog (const about_dialog &);

public:
    virtual ~about_dialog ();

    GtkWidget *to_widget () const
    {
        return dialog;
    }

protected:
    void initialize (GtkWindow *parent);
};

#endif

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-offsets: ((substatement-open . 0))
 * End:
 */
