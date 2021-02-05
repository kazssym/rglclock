// clock.h
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

#ifndef CLOCK_H
#define CLOCK_H 1

#include "gl_context.h"
#include "utils.h"
#include "g_ptr.h"
#include <module.h>
#include <gtk/gtk.h>
#include <gdk/gdktypes.h>
#include <time.h>
#include <memory>

#include "glgtk.h"

using glgtk::glgtk_context;

class glclock;

/* The options dialog.  */
class clock_options_dialog: public options_dialog
{
public:
    class general_options_page
        : public options_page
    {
    private:
        glclock *target;
    public:
        explicit general_options_page(glclock *);
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
    explicit clock_options_dialog(glclock *);
};

extern "C" gboolean handle_timeout(gpointer data) noexcept;
extern "C" gboolean handle_button_press_event(GtkWidget *widget,
    GdkEventButton *event, gpointer data) noexcept;
extern "C" gboolean handle_button_release_event(GtkWidget *widget,
    GdkEventButton *event, gpointer data) noexcept;

/* Clock application.  */
class glclock
{
    friend gboolean handle_button_press_event(GtkWidget *widget,
        GdkEventButton *event, gpointer data) noexcept;
    friend gboolean handle_button_release_event(GtkWidget *widget,
        GdkEventButton *event, gpointer data) noexcept;

public:
    /* Call-back interface that is used to implement hooks about
     * options.  */
    struct listener
    {
        virtual void options_changed(glclock *) = 0;
    };

private:
    int _update_rate;

private:
    std::unique_ptr<module> _module;

private:
    guint _update_timeout {};

private:
    std::vector<listener *> _listeners;

private:
    g_ptr<GtkWidget> _widget;

private:
    std::unique_ptr<glgtk_context> _context;

public:
    /* Constructs this clock with default properties.  */
    glclock();

    glclock(const glclock &) = delete;

public:
    /* Destructs this clock.  */
    virtual ~glclock();

public:
    void operator =(const glclock &) = delete;

public:
    int update_rate() const
    {
        return _update_rate;
    }

public:
    void set_update_rate(int rate);

protected:
    void reset_timeout();

public:
    void add_listener(listener *);

public:
    void remove_listener(listener *);

public:
    const g_ptr<GtkWidget> &widget(void) const
    {
        return _widget;
    }

public:
    void update();

public:
    /* Returns the best visual for this class.  */
    static GdkVisual *best_visual();

protected:
    static void remove_widget(GtkWidget *, gpointer);

private:
    double rot_velocity;
    double rot_x, rot_y, rot_z;
    double press_x, press_y;

public:
    /* Sets modules's property NAME to VALUE.  */
    void set_module_prop(const std::string &name, const std::string &value)
    {_module->set_prop(name, value);}

    GtkWidget *create_widget();

public:
    /* Shows the `Options' dialog.  */
    void show_options_dialog(GtkWindow *);
};

#endif

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-offsets: ((substatement-open . 0) (arglist-intro . +) (arglist-close . 0))
 * End:
 */
