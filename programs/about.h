// about.h
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

#ifndef aboutH
#define aboutH 1

#include <gtk/gtk.h>

class about_dialog
{
private:
    GtkWindow *_parent;
    GtkWidget *dialog;
    GtkWidget *label1;
    GtkWidget *label2;
    GtkWidget *label3;
    GtkWidget *ok_button;

public:
    explicit about_dialog (GtkWindow *parent);
    about_dialog (const about_dialog &object);

public:
    virtual ~about_dialog (void);

    GtkWidget *widget (void) const
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
 * c-file-offsets: ((substatement-open . 0) (arglist-intro . +) (arglist-close . 0))
 * End:
 */
