/* rglclock - Rotating GL Clock.
   Copyright (C) 1998, 1999 Hypercore Software Design, Ltd.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.

   As a special exception, you may copy and distribute this program
   linked with an OpenGL library in executable form without the source
   code for the library, only if such distribution is not prohibited
   by the library's license.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#undef const

#include "glclock.h"
#include "module.h"
#include "gdkgl.h"

#include <gtk/gtk.h>
#include <GL/glu.h>
#include <libintl.h>
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#include <math.h>
#include <algorithm>
#include <stdexcept>

#ifdef HAVE_NANA_H
# include <nana.h>
# include <cstdio>
#else
# include <cassert>
# define I assert
#endif

#define _(MSG) gettext(MSG)

using namespace std;

#define TIMEOUT_RES 1000
#define rate_to_interval(rate) (TIMEOUT_RES / (rate))

#ifndef DEFAULT_TIMEOUT_RATE
# define DEFAULT_TIMEOUT_RATE 10
#endif

GtkWidget *
glclock::create_widget()
{
  GtkWidget *drawing_area = gtk_drawing_area_new();
  gtk_drawing_area_size(GTK_DRAWING_AREA(drawing_area), 100, 100);
  gtk_signal_connect(GTK_OBJECT(drawing_area), "destroy",
		     GTK_SIGNAL_FUNC(remove_widget), this);
  gtk_widget_set_events(drawing_area,
			GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
  gtk_signal_connect(GTK_OBJECT(drawing_area), "expose_event",
		     GTK_SIGNAL_FUNC(handle_expose_event), this);
  gtk_signal_connect(GTK_OBJECT(drawing_area), "button_press_event",
		     GTK_SIGNAL_FUNC(handle_button_event), this);
  gtk_signal_connect(GTK_OBJECT(drawing_area), "button_release_event",
		     GTK_SIGNAL_FUNC(handle_button_event), this);

  widgets.push_back(drawing_area);
  return drawing_area;
}

void
glclock::set_update_rate(int rate)
{
  if (rate < 1 || rate > 100)
    throw invalid_argument("glclock: update_rate is out of [1,100]");

  timeout_rate = rate;
  long interval = rate_to_interval(rate);
  I(interval > 0);
  gtk_timeout_remove(timeout_id);
  timeout_id = gtk_timeout_add(interval, update, this);

  for (vector<options_callback *>::iterator i = callbacks.begin();
       i != callbacks.end();
       ++i)
    (*i)->options_changed(this);
}

void
glclock::remove_callback(options_callback *callback)
{
  vector<options_callback *>::iterator k
    = find(callbacks.begin(), callbacks.end(), callback);
  if (k != callbacks.end())
    callbacks.erase(k);
}

void
glclock::add_callback(options_callback *callback)
{
  callbacks.push_back(callback);
}

glclock::~glclock ()
{
  for (vector<GtkWidget *>::iterator i = widgets.begin();
       i != widgets.end();
       ++i)
    gtk_signal_disconnect_by_data(GTK_OBJECT(*i), this);

  gdk_gl_context_unref(context);
  gtk_timeout_remove (timeout_id);
  delete m;
}

glclock::glclock ()
  : m (NULL),
    timeout_rate(DEFAULT_TIMEOUT_RATE),
    context (NULL),
    rot_velocity (0),
    rot_x (0), rot_y (1), rot_z (0)
{
  try
    {
      /* Objects are allocated here to avoid leaks when an exception
	 is thrown in the middle of the ctor.  */
      m = new module ();

      timeout_id
	= gtk_timeout_add(rate_to_interval(timeout_rate), update, this);
    }
  catch (...)
    {
      /* These are safe as they are initialized to NULLs before
         entering the try block.  */
      delete m;
      throw;
    }
}

gint
glclock::update (gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);

#if 0
  time_t t;
  time(&t);
#endif

#ifdef HAVE_GETTIMEOFDAY
  double angle = 0;
  {
    // FIXME.  The last update time should be kept in the object.
    static struct timeval tv_last = {0};
    struct timeval tv;
    gettimeofday (&tv, NULL);
    if (tv_last.tv_sec != 0)
      {
	double t = (tv.tv_usec - tv_last.tv_usec) / 1e6;
	t += tv.tv_sec - tv_last.tv_sec;
	angle = object->rot_velocity * t;
      }
    tv_last = tv;
  }
#else /* !HAVE_GETTIMEOFDAY */
  double angle = object->rot_velocity / timeout_rate;
#endif /* !HAVE_GETTIMEOFDAY */
  object->m->rotate (angle * (180. / 3.14159),
		     object->rot_x, object->rot_y, object->rot_z);

  for (vector<GtkWidget *>::iterator i = object->widgets.begin();
       i != object->widgets.end();
       ++i)
    {
      gtk_widget_queue_draw(*i);
#if 0
      if (GTK_WIDGET_DRAWABLE(*i))
	{
	  if (object->context == NULL)
	    {
	      object->context
		= gdk_gl_context_new(gdk_window_get_visual((*i)->window));

	      gdk_gl_make_current((*i)->window, object->context);

	      object->m->init();
	    }

	  gdk_gl_make_current((*i)->window, object->context);

	  int width, height;
	  gdk_window_get_size((*i)->window, &width, &height);
	  object->m->viewport(0, 0, width, height);
	  object->m->draw_clock(localtime(&t));

	  gdk_gl_swap_buffers((*i)->window);
	}
#endif
    }

  return 1;			// Do not remove this callback.
}

gint
glclock::handle_button_event (GtkWidget *widget, GdkEventButton *event,
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

gint
glclock::handle_expose_event (GtkWidget *widget, GdkEventExpose *event,
			      gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);

  if (object->context == NULL)
    {
      object->context
	= gdk_gl_context_new(gdk_window_get_visual(widget->window));

      gdk_gl_make_current(widget->window, object->context);

      object->m->init();
    }

  gdk_gl_make_current(widget->window, object->context);

  int width, height;
  gdk_window_get_size(widget->window, &width, &height);
  object->m->viewport(0, 0, width, height);

  time_t t;
  time(&t);
  object->m->draw_clock(localtime(&t));

  gdk_gl_swap_buffers(widget->window);

  return true;
}

void
glclock::remove_widget(GtkObject *object, gpointer data)
{
  GtkWidget *widget = GTK_WIDGET(object);
  glclock *c = static_cast<glclock *>(data);
  I(c != NULL);

  vector<GtkWidget *>::iterator k
    = find(c->widgets.begin(), c->widgets.end(), widget);
#ifdef LG
  LG(k == c->widgets.end(), "glclock: No widget to remove?\n");
#endif
  if (k != c->widgets.end())
    c->widgets.erase(k);
}

GdkVisual *
glclock::best_visual()
{
  int attr[] = {GDK_GL_RGBA,
		GDK_GL_DOUBLEBUFFER,
		GDK_GL_DEPTH_SIZE, 16,
		GDK_GL_NONE};
  GdkVisual *v = gdk_gl_choose_visual(attr);

  return v;
}

