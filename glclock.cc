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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#define _GNU_SOURCE 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#undef const

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
  if (context != NULL)
    gdk_gl_context_unref (context);
  gtk_widget_unref (drawing_area);
  delete m;
}

glclock::glclock ()
  : m (new module),
    drawing_area (gtk_drawing_area_new ()),
    context (NULL),
    rot_velocity (90),
    rot_x (0), rot_y (1), rot_z (0)
{
  gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area), 100, 100);
  gtk_signal_connect_after (GTK_OBJECT (drawing_area), "realize",
			    reinterpret_cast <GtkSignalFunc> (create_context),
			    this);
  gtk_signal_connect (GTK_OBJECT (drawing_area), "configure_event",
		      reinterpret_cast <GtkSignalFunc> (handle_configure_event),
		      this);
  gtk_signal_connect (GTK_OBJECT (drawing_area), "expose_event",
		      reinterpret_cast <GtkSignalFunc> (handle_expose_event),
		      this);
  gtk_widget_show (drawing_area);

  time_t now = time (NULL);
  tm = *localtime (&now);

  timeout_id = gtk_timeout_add (100,
				reinterpret_cast <GtkFunction> (update),
				this);
}

gint
glclock::update (gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);

  time_t now = time (NULL);
  object->tm = *localtime (&now);

  object->m->rotate (object->rot_velocity / 10.,
		     object->rot_x, object->rot_y, object->rot_z);
  gtk_widget_draw (object->drawing_area, NULL);

  return 1;			// Do not remove this callback.
}

gint
glclock::handle_expose_event (GtkWidget *widget, GdkEventExpose *event,
			      gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);
  g_assert (object->drawing_area == widget);

  gdk_gl_set_current (object->context, widget->window);

  object->m->draw_clock (&object->tm);
  gdk_gl_swap_buffers (widget->window);

  gdk_gl_unset_current ();

  return 0;
}

gint
glclock::handle_configure_event (GtkWidget *widget, GdkEventConfigure *event,
				 gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);
  g_assert (object->drawing_area == widget);

  if (object->context != NULL)
    {
      gdk_gl_set_current (object->context, widget->window);

      object->m->viewport (event->x, event->y,
			   event->width, event->height);

      gdk_gl_unset_current ();
    }

  return 0;
}

void
glclock::create_context (GtkWidget *widget, gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);
  g_assert (object->drawing_area == widget);

  if (object->context != NULL)
    gdk_gl_context_unref (object->context);
  object->context = gdk_gl_context_new (gdk_window_get_visual (widget->window));

  gdk_gl_set_current (object->context, widget->window);

  object->m->init ();

  gdk_gl_unset_current ();
}
