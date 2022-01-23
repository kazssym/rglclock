// clock.h
// Copyright (C) 1998-2007 Hypercore Software Design, Ltd.
// Copyright (C) 2021-2022 Kaz Nishimura
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

#ifndef CLOCK_H
#define CLOCK_H 1

#include "utils.h"
#include "glgdkx.h"
#include "g_ptr.h"
#include <gtk/gtk.h>
#include <sys/time.h>
#include <array>
#include <memory>

using glgdkx::glgdkx_context;

#if 0
class movement;

/* The options dialog.  */
class clock_options_dialog: public options_dialog
{
public:
    class general_options_page
        : public options_page
    {
    private:
        movement *target;
    public:
        explicit general_options_page(movement *);
    public:
        GtkWidget *create_widget();

        /* Updates the state of subwidgets.  */
        void update(GtkWidget *page);

        /* Applies the current state.  */
        void apply(GtkWidget *page);
    };
private:
    general_options_page general;
public:
    explicit clock_options_dialog(movement *);
};
#endif

/* Clock movement. */
class movement
{
public:

    static const int DEFAULT_UPDATE_RATE;

private:

    int _update_rate = DEFAULT_UPDATE_RATE;

    guint _update_timeout = 0;

    g_ptr<GtkWidget> _widget = g_ptr<GtkWidget>(gtk_gl_area_new());

    g_ptr<GtkWidget> _menu;

    GLfloat _rate = 0;
    std::array<GLfloat, 3> _axis;

    GLfloat _attitude[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    };

    struct timeval _last_updated {};

    decltype(GdkEventButton::x) _x0;
    decltype(GdkEventButton::y) _y0;

public:

    // Constructors.

    /* Constructs this clock with default properties.  */
    movement();

    movement(const movement &) = delete;


    // Destructors.

    /* Destructs this clock.  */
    virtual ~movement();


    // Assignment operators.

    void operator =(const movement &) = delete;


    int update_rate() const
    {
        return _update_rate;
    }

    void set_update_rate(int rate);

    const g_ptr<GtkWidget> &widget(void) const
    {
        return _widget;
    }

protected:

    void reset_timeout();

public:

    void update() const;

    void realize(GtkWidget *widget) const;

    void render();

protected:

    void rotate(GLfloat angle);

public:

    void begin_drag(GtkWidget *widget, const GdkEvent *event);

    void end_drag(GtkWidget *widget, const GdkEvent *event);

    void popup_menu(GtkWidget *widget, const GdkEvent *event) const;

    /* Shows the `Options' dialog.  */
    void show_options_dialog(GtkWindow *);
};

#endif
