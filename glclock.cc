/* rglclock - R'ing GL Clock.
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
#include <GL/gl.h>
#include <GL/glu.h>

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
}

glclock::glclock ()
  : drawing_area (gtk_drawing_area_new ()),
    context (NULL)
{
  gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area), 100, 100);
  gtk_signal_connect_after (GTK_OBJECT (drawing_area), "realize",
			    reinterpret_cast <GtkSignalFunc> (create_context),
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

void
draw_clock (struct tm *tm)
{
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  gluLookAt (0, 0, 100,
	     0, 0, 0,
	     0, 1, 0);

  glClear (GL_COLOR_BUFFER_BIT);

  /* Short hand.  */
  glPushMatrix ();
  glRotated (((tm->tm_hour * 60 + tm->tm_min) * 60
	      + tm->tm_sec) / 120.0, 0, 0, -1);
  glBegin (GL_TRIANGLES);
  glNormal3d (0.333, 0., 1.);
  glVertex3d (3., 3., 1.);
  glVertex3d (0., 25., 2.);
  glVertex3d (0., 0., 2.);
  glNormal3d (-0.333, 0., 1.);
  glVertex3d (0., 0., 2.);
  glVertex3d (0., 25., 2.);
  glVertex3d (-3., 3., 1.);
  glEnd ();
  glPopMatrix ();

  /* Long hand.  */
  glPushMatrix ();
  glRotated ((tm->tm_min * 60 + tm->tm_sec) / 10.0, 0, 0, -1);
  glBegin (GL_TRIANGLES);
  glNormal3d (0.5, 0., 1.);
  glVertex3d (2., 2., 3.);
  glVertex3d (0., 40., 4.);
  glVertex3d (0., 0., 4.);
  glNormal3d (-0.5, 0., 1.);
  glVertex3d (0., 0., 4.);
  glVertex3d (0., 40., 4.);
  glVertex3d (-2., 2., 3.);
  glEnd ();
  glPopMatrix ();
}

void
glclock::update (gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);

  time_t now = time (NULL);
  object->tm = *localtime (&now);

  gtk_widget_draw (object->drawing_area, NULL);
}

gint
glclock::handle_expose_event (GtkWidget *widget, GdkEventExpose *event,
			      gpointer opaque)
{
  glclock *object = static_cast <glclock *> (opaque);
  g_assert (object != NULL);
  g_assert (object->drawing_area == widget);

  gdk_gl_set_current (object->context, widget->window);

  /* glViewport can be in configure handler?  */
  glViewport (widget->allocation.x, widget->allocation.y,
	      widget->allocation.width, widget->allocation.height);
  draw_clock (&object->tm);
  gdk_gl_swap_buffers (widget->window);

  gdk_gl_unset_current ();

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

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  {
    GLfloat position[] = {-1., 1., 1., 0.};
    glLightfv (GL_LIGHT0, GL_POSITION, position);
  }

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  //gluPerspective (35.0, 1., 1., 1000.);
  glFrustum (-5., 5., -5., 5., 10., 200.);

  gdk_gl_unset_current ();
}
