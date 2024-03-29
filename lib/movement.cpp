// movement.cpp
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "clock.h"

#include "simple.h"
#include <mat4.h>
#include <GL/gl.h>
#include <gettext.h>
#include <array>
#include <memory>
#include <stdexcept>
#include <cmath>
#include <cassert>

using std::array;
using std::cos;
using std::invalid_argument;
using std::make_unique;
using std::sin;
using std::sqrt;

#define _(String) gettext(String)
#define N_(String) gettext_noop(String)


extern "C" gboolean handle_timeout(gpointer data) noexcept;
extern "C" gboolean handle_realize(GtkWidget *widget, gpointer data) noexcept;
extern "C" gboolean handle_render(GtkGLArea *widget, GdkGLContext *context,
    gpointer data) noexcept;
extern "C" gboolean handle_button_press_event(GtkWidget *widget,
    GdkEventButton *event, gpointer data) noexcept;
extern "C" gboolean handle_button_release_event(GtkWidget *widget,
    GdkEventButton *event, gpointer data) noexcept;


const int movement::DEFAULT_UPDATE_RATE = 20;

movement::movement()
{
    gtk_gl_area_set_has_depth_buffer(GTK_GL_AREA(&*_widget), true);

    g_signal_connect(&*_widget, "realize", G_CALLBACK(handle_realize), this);
    g_signal_connect(&*_widget, "render", G_CALLBACK(handle_render), this);

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
    }
}

void movement::reset_timeout()
{
    if (_update_timeout != 0) {
        g_source_remove(_update_timeout);
        _update_timeout = 0;
    }

    unsigned int interval = 1000 / _update_rate;
    assert(interval > 0);
    _update_timeout = g_timeout_add(interval, handle_timeout, this);
}

void movement::update() const
{
    gtk_gl_area_queue_render(GTK_GL_AREA(&*_widget));
}

void movement::realize(GtkWidget *widget) const
{
    gtk_gl_area_make_current(GTK_GL_AREA(widget));

    simple_init();
}

void movement::render()
{
    struct timeval now {};
    gettimeofday(&now, nullptr);

    GLfloat angle = 0;
    if (_last_updated.tv_sec != 0)
    {
        auto sec = now.tv_sec - _last_updated.tv_sec;
        auto usec = sec * 1000000 + now.tv_usec - _last_updated.tv_usec;
        angle = _rate * (float(usec) / 1000000);
    }
    _last_updated = now;
    rotate(angle);

    simple_draw_clock(_attitude);
}

void movement::rotate(GLfloat angle)
{
    GLfloat rotation[4][4] = {};
    mat4_rotate(angle, _axis[0], _axis[1], _axis[2], rotation);

    GLfloat matrix[4][4] = {};
    mat4_multiply(rotation, _attitude, matrix);
    mat4_copy(matrix, _attitude);
}

void movement::begin_drag(GtkWidget *widget, const GdkEventButton *event)
{
    gtk_grab_add(widget);

    _x0 = event->x;
    _y0 = event->y;
}

void movement::end_drag(GtkWidget *widget, const GdkEventButton *event)
{
    gtk_grab_remove(widget);

    GtkAllocation allocation {};
    gtk_widget_get_allocation(widget, &allocation);

    auto v = array<GLfloat, 2> {
        GLfloat(event->x - _x0) / allocation.width,
        GLfloat(event->y - _y0) / allocation.height,
    };
    _rate = sqrt(v[0] * v[0] + v[1] * v[1]);
    _axis = {v[1], v[0], 0};
}

void movement::popup_menu(GtkWidget *, const GdkEvent *event) const
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

gboolean handle_realize(GtkWidget *widget, gpointer data) noexcept
{
    const auto *m = static_cast<movement *>(data);
    assert(m != nullptr);
    m->realize(widget);

    return true;
}

gboolean handle_render([[maybe_unused]] GtkGLArea *widget,
    [[maybe_unused]] GdkGLContext *context, gpointer data) noexcept
{
    auto *m = static_cast<movement *>(data);
    assert(m != nullptr);
    m->render();

    return true;
}

gboolean handle_button_press_event(GtkWidget *widget,
    GdkEventButton *event, gpointer data) noexcept
{
    auto &&m = static_cast<movement *>(data);

    assert(event->type == GDK_BUTTON_PRESS);
    switch (event->button) {
    case 1:
        m->begin_drag(widget, event);
        return true;
    case 3:
        return false;
    default:
        return false;
    }
}

gboolean handle_button_release_event(GtkWidget *widget,
    GdkEventButton *event, gpointer data) noexcept
{
    auto &&m = static_cast<movement *>(data);

    assert(event->type == GDK_BUTTON_RELEASE);
    switch (event->button)
    {
    case 1:
        m->end_drag(widget, event);
        return true;
    case 3:
        m->popup_menu(widget, reinterpret_cast<GdkEvent *>(event));
        return true;
    default:
        return false;
    }
}
