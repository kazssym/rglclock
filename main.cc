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

#include <signal.h>
#include <gtk/gtk.h>

#include "glclock.h"

static glclock *glc;

/* Handle a signal by exit.  */
static void
handle_sig_by_exit (int sig)
{
  delete glc;
  glc = NULL;
  gtk_exit (0xff);
}

/* Set signal handlers.  */
static void
set_handlers ()
{
  struct sigaction sa;
  sa.sa_handler = handle_sig_by_exit;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction (SIGHUP, &sa, NULL);
  sigaction (SIGINT, &sa, NULL);
  sigaction (SIGTERM, &sa, NULL);
}

int
main (int argc, char **argv)
{
  gtk_set_locale ();
  gtk_init (&argc, &argv);

  set_handlers ();

  GtkWidget *toplevel = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  glc = new glclock ();
  gtk_widget_ref (*glc);
  gtk_container_add (GTK_CONTAINER (toplevel), *glc);

  gtk_widget_show (toplevel);
  gtk_main ();

  return 0;
}
