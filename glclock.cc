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

#define _GNU_SOURCE 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#undef const

#include <math.h>
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#include <gtk/gtk.h>
#include <gdkGL/gdkGL.h>
#include <GL/glu.h>
#include "module.h"

#include "glclock.h"

glclock::operator GtkWidget *() const
{
  return drawing_area;
}

glclock::~glclock ()
{
  gtk_timeout_remove (timeout_id);
  gtk_widget_unref (drawing_area);
  delete m;
}

glclock::glclock ()
  : m (new module),
    drawing_area (gtk_drawing_area_new ()),
    context (NULL),
    rot_velocity (0),
    rot_x (0), rot_y (1), rot_z (0)
{
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

  time (&t);

  timeout_id = gtk_timeout_add (100,
				reinterpret_cast <GtkFunction> (update),
				this);
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
      gdk_gl_set_current (object->context, widget->window);

      object->m->draw_clock (localtime (&object->t));
      gdk_gl_swap_buffers (widget->window);

      gdk_gl_unset_current ();
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

  if (event->type == GDK_BUTTON_PRESS && event->button == 1)
    {
      object->press_x = event->x;
      object->press_y = event->y;
      gtk_grab_add (widget);
    }
  else if (event->type == GDK_BUTTON_RELEASE && event->button == 1)
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

      gdk_gl_set_current (object->context, widget->window);

      object->m->init ();
    }
  else
    gdk_gl_set_current (object->context, widget->window);

  object->m->viewport (event->x, event->y,
		       event->width, event->height);

  gdk_gl_unset_current ();

  return 0;
}
