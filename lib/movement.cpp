// glclock.cpp
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

#include "clock.h"

#include <GL/glu.h>
#include <gtk/gtk.h>
#include <gettext.h>
#include <sys/time.h>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <cassert>

#ifndef DEFAULT_UPDATE_RATE
#define DEFAULT_UPDATE_RATE 10
#endif

using std::for_each;
using std::invalid_argument;
using std::make_unique;
using std::remove;
using glgdkx::glgdkx_context;

#define _(String) gettext(String)
#define N_(String) gettext_noop(String)

#define TIMEOUT_RES 1000
#define rate_to_interval(rate) (TIMEOUT_RES / (rate))

movement::movement():
    _update_rate {DEFAULT_UPDATE_RATE},
    _module {new module()},
    _widget {gtk_drawing_area_new()}
{
    gtk_widget_set_events(&*_widget,
        GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
    g_signal_connect(&*_widget, "button_press_event",
        G_CALLBACK(handle_button_press_event), this);
    g_signal_connect(&*_widget, "button_release_event",
        G_CALLBACK(handle_button_release_event), this);

    g_ptr<GMenu> model {g_menu_new()};
    g_menu_append(&*model, _("_About..."), "app.about");
    g_menu_append(&*model, _("E_xit"), "app.exit");
    _menu.reset(gtk_menu_new_from_model(G_MENU_MODEL(&*model)));
    gtk_menu_attach_to_widget(GTK_MENU(&*_menu), &*_widget, nullptr);

    reset_timeout();
}

movement::~movement()
{
    if (&*_widget != nullptr) {
        g_signal_handlers_disconnect_by_data(&*_widget, this);
    }

    if (_update_timeout != 0) {
        g_source_remove(_update_timeout);
        _update_timeout = 0;
    }
}

void movement::set_update_rate(int rate)
{
    if (rate < 1 || rate > 100) {
        throw invalid_argument("Out of range");
    }

    if (rate != _update_rate) {
        _update_rate = rate;
        reset_timeout();

        for_each(_listeners.begin(), _listeners.end(),
            [this](auto &&listener) {
                listener->options_changed(this);
            });
    }
}

void movement::reset_timeout()
{
    if (_update_timeout != 0) {
        g_source_remove(_update_timeout);
        _update_timeout = 0;
    }

    unsigned int interval = rate_to_interval(_update_rate);
    assert(interval > 0);
    _update_timeout = g_timeout_add(interval, handle_timeout, this);
}

void movement::add_listener(listener *listener)
{
    _listeners.push_back(listener);
}

void movement::remove_listener(listener *listener)
{
    auto &&last = remove(_listeners.begin(), _listeners.end(), listener);
    _listeners.erase(last, _listeners.end());
}

void movement::update()
{
    if (&*_widget == nullptr || !gtk_widget_get_realized(&*_widget)) {
        return;
    }

    auto &&window = gtk_widget_get_window(&*_widget);
    if (_context == nullptr) {
        _context = make_unique<glgdkx_context>(window);
        _context->make_current(window);
        _module->init();
    }

    double angle = 0;
    // FIXME.  The last update time should be kept in the object.
    static struct timeval tv_last = {0};
    struct timeval tv;
    gettimeofday (&tv, NULL);
    if (tv_last.tv_sec != 0)
    {
        double t = (tv.tv_usec - tv_last.tv_usec) / 1e6;
        t += tv.tv_sec - tv_last.tv_sec;
        angle = _velocity * t;
    }
    tv_last = tv;
    _module->rotate((180 / M_PI) * angle, _axis[0], _axis[1], _axis[2]);

    _module->viewport(0, 0,
        gdk_window_get_width(window), gdk_window_get_height(window));
    _module->draw_clock ();

    _context->swap_buffers(window);
}

void movement::begin_drag(GtkWidget *, GdkEventButton *event)
{
    _x0 = event->x;
    _y0 = event->y;
}

void movement::end_drag(GtkWidget *widget, GdkEventButton *event)
{
    GtkAllocation allocation {};
    gtk_widget_get_allocation(widget, &allocation);

    double v[2] {
        double(event->x - _x0) / allocation.width,
        double(event->y - _y0) / allocation.height,
    };
    _velocity = sqrt(v[0] * v[0] + v[1] * v[1]);
    _axis = {v[1], v[0], 0};
}

void movement::popup_menu(GtkWidget *, GdkEvent *event) const
{
    gtk_menu_popup_at_pointer(GTK_MENU(&*_menu), event);
}

gboolean handle_timeout(gpointer data) noexcept
{
    auto &&clock = static_cast<movement *>(data);
    assert(clock != NULL);
    clock->update();

    return true;
}

gboolean handle_button_press_event(GtkWidget *widget,
    GdkEventButton *event, gpointer data) noexcept
{
    movement *m = static_cast<movement *>(data);

    switch (event->button) {
    case 1:
        gtk_grab_add(widget);
        m->begin_drag(widget, event);
        return true;
    case 3:
        m->popup_menu(widget, reinterpret_cast<GdkEvent *>(event));
        return true;
    default:
        return false;
    }
}

gboolean handle_button_release_event(GtkWidget *widget,
    GdkEventButton *event, gpointer data) noexcept
{
    movement *clock = static_cast<movement *>(data);

    switch (event->button)
    {
    case 1:
        gtk_grab_remove(widget);
        clock->end_drag(widget, event);
        return true;
    default:
        return false;
    }
}
