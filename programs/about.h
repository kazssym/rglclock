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

#ifndef ABOUT_H
#define ABOUT_H 1

#include "g_ptr.h"
#include <gtk/gtk.h>

class about_dialog
{
private:

    g_ptr<GtkWidget> _widget {gtk_dialog_new()};

public:

    explicit about_dialog(GtkWindow *parent = nullptr);

    about_dialog(const about_dialog &) = delete;


    void operator =(const about_dialog &) = delete;


    int show_modal() const;
};

#endif
