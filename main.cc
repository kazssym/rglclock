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
   02111-1307, USA.

   As a special exception, you may copy and distribute this program
   linked with an OpenGL library in executable form without the source
   code for the library, only if such distribution is not prohibited
   by the library's license.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
// C++ must have them.
#undef const
#undef inline

#include "glclock.h"

#include "autowidget.h"
#include <gtk/gtk.h>
#include <libintl.h>
#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <signal.h>
#include <cstdio>
#include <string>
#include <exception>

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
#ifdef PKGDATADIR
      gtk_rc_parse(PKGDATADIR "/gtkrc");
#endif /* PKGDATADIR */

      const char *homedir = getenv("HOME");
      if (homedir != NULL)
	{
	  string gtkrc (homedir);
	  gtkrc.append("/.rglclock");
	  gtkrc.append("/gtkrc");
	  gtk_rc_parse(gtkrc.c_str());
	}
    }
} // (unnamed namespace)

static glclock *glc;

int
main (int argc, char **argv)
{
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

  try
    {
      parse_gtkrcs();

      static int attr[] = {GDK_GL_RGBA,
			   GDK_GL_DOUBLEBUFFER,
			   GDK_GL_DEPTH_SIZE, 4,
			   GDK_GL_NONE};
      GdkVisual *visual = gdk_gl_choose_visual(attr);
      gtk_widget_set_default_colormap(gdk_colormap_new(visual, TRUE));
      gtk_widget_set_default_visual(visual);

      glc = new glclock ();

      GtkWidget *toplevel = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      gtk_signal_connect(GTK_OBJECT(toplevel), "delete_event",
			 GTK_SIGNAL_FUNC(gtk_main_quit), glc);

      GtkObject_ptr<GtkItemFactory> ifactory
	(gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<Window>", NULL));
      GtkItemFactoryEntry entries[]
	=
      {
	{_("/_File/E_xit"), NULL, NULL, 1, "<Item>"},
	{_("/_File/"), NULL, NULL, 0, "<Separator>"},
	{_("/_File/_About..."), NULL, NULL, 2, "<Item>"}
      };
      gtk_item_factory_create_items(ifactory.get(), 3, entries, glc);
      // Unimplemented menu items.
      gtk_widget_set_sensitive(gtk_item_factory_get_widget_by_action(ifactory.get(),
								     1),
			       FALSE);
      gtk_widget_set_sensitive(gtk_item_factory_get_widget_by_action(ifactory.get(),
								     2),
			       FALSE);

      {
	GtkObject_ptr<GtkWidget> box1(gtk_vbox_new(FALSE, 0));
	{
	  gtk_widget_show(ifactory->widget);
	  gtk_box_pack_start(GTK_BOX(box1.get()), ifactory->widget,
			     FALSE, FALSE, 0);

	  GtkObject_ptr<GtkWidget> content (glc->create_widget());
	  gtk_widget_show(content.get());
	  gtk_box_pack_start(GTK_BOX(box1.get()), content.get(),
			     TRUE, TRUE, 0);
	}
	gtk_widget_show(box1.get());
	gtk_container_add(GTK_CONTAINER(toplevel), box1.get());
      }

      gtk_widget_show (toplevel);
      gtk_main ();

      gtk_widget_hide(toplevel);
      delete glc;
      gtk_widget_destroy(toplevel);
    }
  catch (exception &x)
    {
      fprintf(stderr, _("%s: Unhandled exception: %s\n"), argv[0], x.what());
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
