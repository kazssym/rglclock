/* rglclock - Rotating GL Clock.
   Copyright (C) 1998 Hypercore Software Design, Ltd.

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
   02111-1307, USA.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#undef const

#include "glclock.h"
#include "module.h"
#include "gdkgl.h"

#include <gtk/gtk.h>
#include <GL/glu.h>
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#include <math.h>

void
glclock::menu_listener::menu_activate (GtkWidget *widget, gpointer opaque)
{
  const glclock::menu_listener *listener =
    static_cast <glclock::menu_listener *> (opaque);
  (listener->target->*listener->activate) (widget);
}

glclock::operator GtkWidget *() const
{
  return drawing_area;
}

void show_about(GtkWidget *);	// XXX

void
glclock::describe (GtkWidget *widget)
{
  show_about(gtk_widget_get_toplevel(drawing_area));
}

glclock::~glclock ()
{
  gtk_timeout_remove (timeout_id);
  gtk_menu_factory_destroy (menu_factory);
  gtk_widget_unref (drawing_area);
  delete m;
}

glclock::glclock ()
  : m (NULL),
    drawing_area (NULL),
    menu_factory (NULL),
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
      menu_factory = gtk_menu_factory_new (GTK_MENU_FACTORY_MENU);

      gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area), 100, 100);

      gtk_widget_set_events (drawing_area, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
      gtk_signal_connect (GTK_OBJECT (drawing_area), "configure_event",
			  reinterpret_cast <GtkSignalFunc> (handle_configure_event),
			  this);
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

      about.target = this;
      about.activate = &glclock::describe;
      GtkMenuEntry entries[] =
      {
	/* {path, accelerator, callback, callback_data, widget} */
	{"Reset Rotation", NULL, NULL, NULL, NULL},
	{"<separator>", NULL, NULL, NULL, NULL},
	{"Properties...", NULL, NULL, NULL, NULL},
	{"About...", NULL, menu_listener::menu_activate, &about, NULL},
	{"<separator>", NULL, NULL, NULL, NULL},
	{"Exit", NULL, menu_quit, this, NULL}
      };
      gtk_menu_factory_add_entries (menu_factory, entries, 6);

      time (&t);

      timeout_id = gtk_timeout_add (100,
				    reinterpret_cast <GtkFunction> (update),
				    this);
    }
  catch (...)
    {
      /* These are safe as they are initialized to NULLs before
         entering the try block.  */
      if (menu_factory != NULL)
	gtk_menu_factory_destroy (menu_factory);
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

gint
glclock::handle_configure_event (GtkWidget *widget, GdkEventConfigure *event,
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
  else
    gdk_gl_make_current (widget->window, object->context);

  object->m->viewport (event->x, event->y,
		       event->width, event->height);

  return 0;
}

void
glclock::menu_quit (GtkWidget *widget, gpointer opaque)
{
  gtk_main_quit ();
}
