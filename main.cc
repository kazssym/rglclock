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

#include <gtk/gtk.h>
#include <libintl.h>
#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <signal.h>
#include <string>

#ifdef SESSION
# include <gdk/gdkx.h>
# include <algorithm>
#endif

#define _(MSG) gettext(MSG)

using namespace std;

namespace
{
  int opt_version = 0;

#ifdef HAVE_GETOPT_LONG
  const struct option longopts[] =
  {
    {"version", no_argument, &opt_version, 1},
    {NULL, 0, NULL, 0}
  };
#endif

  bool parse_options(int argc, char **argv)
    {
      int optc;
      do
	{
#ifdef HAVE_GETOPT_LONG
	  int index;
	  optc = getopt_long(argc, argv, "", longopts, &index);
#else /* not HAVE_GETOPT_LONG */
	  optc = getopt(argc, argv, "");
#endif /* not HAVE_GETOPT_LONG */

	  switch (optc)
	    {
	    case 0:		// long option
	      break;
	    case '?':
	      return false;
	    }
	}
      while (optc != -1);

      return true;
    }

  void parse_gtkrcs()
    {
      const char *homedir = getenv("HOME");
      if (homedir != NULL)
	{
	  string gtkrc (homedir);
	  gtkrc.append("/.rglclock");
	  gtkrc.append("/gtkrc");
	  gtk_rc_parse(gtkrc.c_str());
	}
    }
} // *unnamed*

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
#endif /* SESSION */

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

  if (!parse_options(argc, argv))
    return EXIT_FAILURE;

  /* Initialize NLS.  */
#ifdef LOCALEDIR
  bindtextdomain(PACKAGE, LOCALEDIR);
#endif
  textdomain(PACKAGE);

  if (opt_version)
    {
      printf("%s %s\n", PACKAGE, VERSION);
      return EXIT_SUCCESS;
    }

  ATEXIT (clean);

  parse_gtkrcs();

  static int attr[] = {GDK_GL_RGBA,
		       GDK_GL_DOUBLEBUFFER,
		       GDK_GL_DEPTH_SIZE, 4,
		       GDK_GL_NONE};
  GdkVisual *visual = gdk_gl_choose_visual(attr);
  gtk_widget_set_default_colormap(gdk_colormap_new(visual, TRUE));
  gtk_widget_set_default_visual(visual);

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
