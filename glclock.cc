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

glclock::operator GtkWidget *() const
{
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
}

glclock::~glclock ()
{
  gtk_timeout_remove (timeout_id);
  gtk_object_unref(GTK_OBJECT(menu_factory));
  gtk_widget_unref (drawing_area);
  delete m;
}

glclock::glclock ()
  : m (NULL),
    drawing_area (NULL),
    menu_factory (NULL),
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
      drawing_area = gtk_drawing_area_new ();
      menu_factory = gtk_item_factory_new(GTK_TYPE_MENU, "<Popup>", NULL);

      gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area), 100, 100);

      gtk_widget_set_events (drawing_area, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
      gtk_signal_connect_after(GTK_OBJECT(drawing_area), "realize",
			       GTK_SIGNAL_FUNC(finish_realize), this);
      gtk_signal_connect(GTK_OBJECT(drawing_area), "configure_event",
			  GTK_SIGNAL_FUNC(handle_configure_event), this);
      gtk_signal_connect (GTK_OBJECT (drawing_area), "destroy_event",
			  reinterpret_cast <GtkSignalFunc> (handle_destroy_event),
			  this);
#if 0
      gtk_signal_connect (GTK_OBJECT (drawing_area), "expose_event",
			  reinterpret_cast <GtkSignalFunc> (handle_expose_event),
			  this);
#endif
      gtk_signal_connect (GTK_OBJECT (drawing_area), "button_press_event",
			  reinterpret_cast <GtkSignalFunc> (handle_button_event),
			  this);
      gtk_signal_connect (GTK_OBJECT (drawing_area), "button_release_event",
			  reinterpret_cast <GtkSignalFunc> (handle_button_event),
			  this);
      gtk_widget_show (drawing_area);

      /* {path, accelerator, callback, callback_data, widget} */
      GtkItemFactoryEntry entries[] =
      {
	{_("/Reset Rotation"), NULL, NULL, 1, ""},
	{"/", NULL, NULL, 0, "<Separator>"},
	{_("/Options..."), NULL, (GtkItemFactoryCallback) &edit_options, 2, ""},
	{_("/About..."), NULL, (GtkItemFactoryCallback) &menu_about, 3, ""},
	{"/", NULL, NULL, 0, "<Separator>"},
	{_("/Exit"), NULL, (GtkItemFactoryCallback) &menu_quit, 4, ""}
      };
      gtk_item_factory_create_items(menu_factory, 6, entries, this);

      time (&t);

      timeout_id
	= gtk_timeout_add(rate_to_interval(timeout_rate), update, this);
    }
  catch (...)
    {
      /* These are safe as they are initialized to NULLs before
         entering the try block.  */
      if (menu_factory != NULL)
	gtk_object_unref(GTK_OBJECT(menu_factory));
      if (drawing_area != NULL)
	gtk_widget_unref (drawing_area);
      delete m;
      throw;
    }
}

gint
glclock::update (gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);

  time (&object->t);

#ifdef HAVE_GETTIMEOFDAY
  double angle = 0;
  {
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
  double angle = object->rot_velocity / 10;
#endif /* !HAVE_GETTIMEOFDAY */
  object->m->rotate (angle * (180. / 3.14159),
		     object->rot_x, object->rot_y, object->rot_z);

  GtkWidget *widget = object->drawing_area;
  /* If a context is available, a window must also be available here.  */
  if (object->context != NULL)
    {
      gdk_gl_make_current (widget->window, object->context);

      object->m->draw_clock (localtime (&object->t));
      gdk_gl_swap_buffers (widget->window);
    }

  return 1;			// Do not remove this callback.
}

gint
glclock::handle_button_event (GtkWidget *widget, GdkEventButton *event,
			      gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);
  g_assert (object->drawing_area == widget);

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
    case 3:
      {
	switch (event->type)
	  {
	  case GDK_BUTTON_PRESS:
	    gtk_menu_popup (GTK_MENU (object->menu_factory->widget),
			    NULL, NULL, NULL, NULL,
			    event->button, event->time);
	    return 1;
	  case GDK_BUTTON_RELEASE:
	    gtk_menu_popdown (GTK_MENU (object->menu_factory->widget));
	    return 1;
	  default:
	    break;
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
  g_assert (object->drawing_area == widget);

  return 0;
}

gint
glclock::handle_destroy_event (GtkWidget *widget, GdkEventAny *event,
			       gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);
  g_assert (object->drawing_area == widget);

  if (object->context != NULL)
    {
      gdk_gl_context_unref (object->context);
      object->context = NULL;
    }

  return 0;
}

/* Handles "configure_event"s.  */
gint
glclock::handle_configure_event (GtkWidget *widget, GdkEventConfigure *event,
				 gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);

  if (object->context != NULL)
    {
      gdk_gl_make_current (widget->window, object->context);

      object->m->viewport (event->x, event->y,
			   event->width, event->height);
    }

  return 0;
}

/* Finish realizing.  */
void
glclock::finish_realize(GtkWidget *widget,
			gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);
  g_assert (object->drawing_area == widget);

  if (object->context == NULL)
    {
      object->context = gdk_gl_context_new (gdk_window_get_visual (widget->window));

      gdk_gl_make_current (widget->window, object->context);

      object->m->init ();
    }
}

void
glclock::menu_quit(gpointer, guint, GtkWidget *)
{
  gtk_main_quit ();
}

void
glclock::menu_about(gpointer data, guint, GtkWidget *)
{
  glclock *object = static_cast<glclock *>(data);

  about_dialog about(gtk_widget_get_toplevel(object->drawing_area));
  about.show();
}

void
glclock::edit_options(gpointer data, guint, GtkWidget *)
{
  glclock *object = static_cast <glclock *> (data);

  clock_options_dialog dialog(object);
  GtkWidget *widget = dialog.create_widget();
  gtk_window_set_transient_for(GTK_WINDOW(widget),
			       GTK_WINDOW(gtk_widget_get_toplevel(object->drawing_area)));
  gtk_widget_show(widget);
  gtk_main();
  gtk_widget_destroy(widget);
}

