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

#include <signal.h>
#include <gtk/gtk.h>
#ifdef SESSION
# include <gdk/gdkx.h>
# include <algorithm>
#endif

#include "glclock.h"

static GtkWidget *toplevel;
static glclock *glc;

static void
clean ()
{
  delete glc;
  glc = NULL;
#if 0
  /* This code may cause SIGILL when interrupted.  */
  if (toplevel != NULL)
    gtk_widget_unref (toplevel);
  toplevel = NULL;
#endif
}

/* Handle "delete_event".  */
static gint
handle_delete_event (GtkWidget *widget, GdkEventAny *event,
		     gpointer opaque)
{
  gtk_main_quit ();

  return 0;
}

#ifdef SESSION
static int argc_save;
static char **argv_save;

static void
set_command (GtkWidget *widget)
{
  int argc_get;
  char **argv_get;

  if (XGetCommand (GDK_WINDOW_XDISPLAY (widget->window),
		   GDK_WINDOW_XWINDOW (widget->window),
		   &argv_get, &argc_get))
    {
      g_message ("WM_COMMAND is already set.\n");
      XFreeStringList (argv_get);
    }
  else
    XSetCommand (GDK_WINDOW_XDISPLAY (widget->window),
		 GDK_WINDOW_XWINDOW (widget->window),
		 argv_save, argc_save);
}
#endif

int
main (int argc, char **argv)
{
#ifdef SESSION
  argc_save = argc;
  argv_save = new char *[argc + 1]; // This will leak just once.
  std::copy_n (argv, argc + 1, argv_save);
#endif

  gtk_set_locale ();
  gtk_init (&argc, &argv);

  ATEXIT (clean);

  toplevel = gtk_window_new (GTK_WINDOW_TOPLEVEL);
#ifdef SESSION
  /* It appears necessary to set WM_COMMAND before the X window is
     mapped for smproxy to pick it. */
  gtk_signal_connect_after (GTK_OBJECT (toplevel), "realize",
			    reinterpret_cast <GtkSignalFunc> (set_command),
			    NULL);
#endif
  gtk_signal_connect (GTK_OBJECT (toplevel), "delete_event",
		      reinterpret_cast <GtkSignalFunc> (handle_delete_event),
		      NULL);

  glc = new glclock ();
  gtk_widget_ref (*glc);
  gtk_container_add (GTK_CONTAINER (toplevel), *glc);

  gtk_widget_show (toplevel);
  gtk_main ();

  return 0;
}
