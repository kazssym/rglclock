/*
 * RGLClock - rotating 3D clock
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

#include "clock.h"

#include <cmath>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <GL/glu.h>
#include <gtk/gtk.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "module.h"
#include "glgtk.h"

#if ENABLE_NLS
#include <libintl.h>
#define _(t) gettext (t)
#else
#define _(t) (t)
#endif

#ifndef DEFAULT_UPDATE_RATE
#define DEFAULT_UPDATE_RATE 10
#endif

using std::vector;
using std::invalid_argument;
using glgtk::glgtk_context;

#define TIMEOUT_RES 1000
#define rate_to_interval(rate) (TIMEOUT_RES / (rate))

static gboolean handle_timeout (gpointer data) throw ();
static gboolean handle_button_press_event (
    GtkWidget *widget, GdkEventButton *event, gpointer data) throw ();
static gboolean handle_button_release_event (
    GtkWidget *widget, GdkEventButton *event, gpointer data) throw ();

glclock::glclock (void)
    : m (NULL),
      options (this),
      rot_velocity (0),
      rot_x (0), rot_y (1), rot_z (0)
{
    _update_rate = DEFAULT_UPDATE_RATE;
    _update_timeout = rate_to_interval (_update_rate); 
    _widget = NULL;
    _context = NULL;

    try
    {
        /* Objects are allocated here to avoid leaks when an exception
           is thrown in the middle of the ctor.  */
        m = new module ();
    }
    catch (...)
    {
        /* These are safe as they are initialized to NULLs before
           entering the try block.  */
        delete m;
        throw;
    }
}

glclock::~glclock (void)
{
    if (_context != NULL)
    {
        delete _context;
    }

    if (_widget != NULL)
    {
        g_signal_handlers_disconnect_matched (_widget, G_SIGNAL_MATCH_DATA,
                                              0, 0, NULL, NULL, this);
        g_object_unref (_widget);
    }

    if (_update_timeout != 0)
    {
        g_source_remove (_update_timeout);
    }

    delete m;
}

void glclock::set_update_rate (int rate)
{
    if (rate < 1 || rate > 100)
    {
        throw invalid_argument ("Out of range");
    }

    if (rate != _update_rate)
    {
        _update_rate = rate;

        if (_update_timeout != 0)
        {
            g_source_remove (_update_timeout);
        }

        long interval = rate_to_interval (_update_rate);
        assert (interval > 0);
        _update_timeout = g_timeout_add (interval, handle_timeout, this);

        for (vector<options_callback *>::iterator i = callbacks.begin();
             i != callbacks.end(); ++i)
        {
            (*i)->options_changed (this);
        }
    }
}

GtkWidget *glclock::widget (void)
{
    if (_update_timeout ==0)
    {
        long interval = rate_to_interval (_update_rate);
        assert (interval > 0);
        _update_timeout = g_timeout_add (interval, handle_timeout, this);
    }

    if (_widget == NULL)
    {
        _widget = gtk_drawing_area_new ();
        gtk_drawing_area_size (GTK_DRAWING_AREA (_widget), 160, 160);
        gtk_widget_set_double_buffered (_widget, false);
        gtk_widget_set_events (
            _widget, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
#if 0
        gtk_signal_connect (GTK_OBJECT (_widget), "expose_event",
                            GTK_SIGNAL_FUNC (&handle_expose_event), this);
#endif
        gtk_signal_connect (GTK_OBJECT (_widget), "button_press_event",
                            GTK_SIGNAL_FUNC (&handle_button_press_event),
                            this);
        gtk_signal_connect (GTK_OBJECT (_widget), "button_release_event",
                            GTK_SIGNAL_FUNC (&handle_button_release_event),
                            this);
        g_object_ref_sink (_widget);
    }

    return _widget;
}

void glclock::update (void)
{
#if 0
    time_t t;
    time (&t);
#endif

    if (_widget == NULL || !GTK_WIDGET_REALIZED (_widget))
    {
        return;
    }

    if (_context == NULL)
    {
        assert (_widget->window != NULL);
        _context = new glgtk_context (_widget->window);
        _context->make_current (_widget->window);
        m->init ();
    }

#ifdef HAVE_GETTIMEOFDAY
    double angle = 0;
    // FIXME.  The last update time should be kept in the object.
    static struct timeval tv_last = {0};
    struct timeval tv;
    gettimeofday (&tv, NULL);
    if (tv_last.tv_sec != 0)
    {
        double t = (tv.tv_usec - tv_last.tv_usec) / 1e6;
        t += tv.tv_sec - tv_last.tv_sec;
        angle = rot_velocity * t;
    }
    tv_last = tv;
#else /* !HAVE_GETTIMEOFDAY */
    double angle = rot_velocity / _update_rate;
#endif /* !HAVE_GETTIMEOFDAY */
    m->rotate (angle * (180 / M_PI), rot_x, rot_y, rot_z);

    int width, height;
    gdk_window_get_size (_widget->window, &width, &height);
    m->viewport (0, 0, width, height);
    m->draw_clock ();

    _context->swap_buffers (_widget->window);
}

gboolean handle_timeout (gpointer data) throw ()
{
    glclock *clock = static_cast<glclock *> (data);
    assert (clock != NULL);
    clock->update ();

    return true;
}

gboolean handle_button_press_event (
    GtkWidget *widget, GdkEventButton *event, gpointer data) throw ()
{
    glclock *clock = static_cast<glclock *> (data);
    g_assert (clock != NULL);

    switch (event->button)
    {
#if 0
    case 1:
        clock->press_x = event->x;
        clock->press_y = event->y;
        gtk_grab_add (widget);
        return true;
#endif
    }

    return false;
}

gboolean handle_button_release_event (
    GtkWidget *widget, GdkEventButton *event, gpointer data) throw ()
{
    glclock *clock = static_cast<glclock *> (data);
    g_assert (clock != NULL);

    switch (event->button)
    {
#if 0
    case 1:
        gtk_grab_remove (widget);
        {
            double vel_x = (double) (event->x - clock->press_x) / widget->allocation.width;
            double vel_y = (double) (event->y - clock->press_y) / widget->allocation.height;
            if (vel_x != 0 || vel_y != 0)
            {
                clock->rot_y = vel_x;
                clock->rot_x = vel_y;
            }
            clock->rot_velocity = sqrt (vel_x * vel_x + vel_y * vel_y);
        }
        return true;
#endif
    }

    return false;
}

#if 0

void glclock::show_options_dialog (GtkWindow *w)
{
    options.act (w);
}

GtkWidget *glclock::create_widget ()
{
    GtkWidget *drawing_area = gtk_drawing_area_new ();
    gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area), 160, 160);
    gtk_widget_set_double_buffered (drawing_area, false);
    gtk_widget_set_events (drawing_area,
                           GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
    gtk_signal_connect (GTK_OBJECT(drawing_area), "destroy",
                        GTK_SIGNAL_FUNC(remove_widget), this);
    gtk_signal_connect (GTK_OBJECT(drawing_area), "expose_event",
                        GTK_SIGNAL_FUNC(handle_expose_event), this);
    gtk_signal_connect (GTK_OBJECT(drawing_area), "button_press_event",
                        GTK_SIGNAL_FUNC(handle_button_event), this);
    gtk_signal_connect (GTK_OBJECT(drawing_area), "button_release_event",
                        GTK_SIGNAL_FUNC(handle_button_event), this);

    widgets.push_back(drawing_area);
    return drawing_area;
}

#endif

void glclock::remove_callback (options_callback *callback)
{
    vector<options_callback *>::iterator k
        = find(callbacks.begin(), callbacks.end(), callback);
    if (k != callbacks.end())
        callbacks.erase(k);
}

void glclock::add_callback (options_callback *callback)
{
    callbacks.push_back(callback);
}

gint glclock::handle_button_event (GtkWidget *widget, GdkEventButton *event,
                                   gpointer opaque)
{
    glclock *object = static_cast <glclock *> (opaque);
    g_assert (object != NULL);

    switch (event->button)
    {
    case 1:
        {
            if (event->type == GDK_BUTTON_PRESS)
            {
                object->press_x = event->x;
                object->press_y = event->y;
                gtk_grab_add (widget);
                return 1;
            }
            else if (event->type == GDK_BUTTON_RELEASE)
            {
                gtk_grab_remove (widget);
                double vel_x = double (event->x - object->press_x) / widget->allocation.width;
                double vel_y = double (event->y - object->press_y) / widget->allocation.height;
                if (vel_x != 0 || vel_y != 0)
                {
                    object->rot_y = vel_x;
                    object->rot_x = vel_y;
                }
                object->rot_velocity = sqrt (vel_x * vel_x + vel_y * vel_y);
                return 1;
            }
        }
        break;
    }

    return 0;
}

#if 0

gint glclock::handle_expose_event (GtkWidget *widget, GdkEventExpose *event,
                                   gpointer opaque)
{
    glclock *object = static_cast <glclock *> (opaque);
    g_assert (object != NULL);

    if (object->context == NULL)
    {
        object->context
            = gl::create_context(gdk_window_get_visual(widget->window));

        gl::make_current(object->context, widget->window);

        object->m->init();
    }

    gl::make_current(object->context, widget->window);

    int width, height;
    gdk_window_get_size(widget->window, &width, &height);
    object->m->viewport(0, 0, width, height);

    object->m->draw_clock();

    gl::swap_buffers(widget->window);

    return true;
}

void glclock::remove_widget (GtkObject *object, gpointer data)
{
    GtkWidget *widget = GTK_WIDGET (object);
    glclock *c = static_cast<glclock *> (data);
    assert (c != NULL);

    vector<GtkWidget *>::iterator k
        = find (c->widgets.begin(), c->widgets.end(), widget);
    if (k != c->widgets.end())
        c->widgets.erase (k);
}

#endif

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-offsets: ((substatement-open . 0) (arglist-intro . +) (arglist-close . 0))
 * End:
 */