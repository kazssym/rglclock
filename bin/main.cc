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
// C++ must have them.
#undef const
#undef inline

#include "profile.h"
#include "glclock.h"

#include "autowidget.h"
#include "getopt.h"
#include <gtk/gtk.h>
#include <libintl.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <sys/stat.h>
#include <signal.h>
#include <cstdio>
#include <string>
#include <exception>

#ifdef HAVE_NANA_H
# include <nana.h>
# include <cstdio>
#else
# include <cassert>
# define I assert
#endif

#define _(MSG) gettext(MSG)

using namespace std;

namespace
{
  int opt_hide_menu_bar = 0;
  int opt_private_colormap = false;
  int opt_help = 0;
  int opt_version = 0;

  const struct option longopts[] =
  {
    {"hide-menu-bar", no_argument, &opt_hide_menu_bar, 1},
    {"private-colormap", no_argument, &opt_private_colormap, true},
    {"help", no_argument, &opt_help, 1},
    {"version", no_argument, &opt_version, 1},
    {NULL, 0, NULL, 0}
  };

  bool parse_options(int argc, char **argv)
    {
      int optc;
      do
	{
	  int index;
	  optc = getopt_long(argc, argv, "m", longopts, &index);

	  switch (optc)
	    {
	    case 'm':
	      opt_hide_menu_bar = 1;
	      break;
	    case 0:		// long option
	      break;
	    case '?':
	      return false;
	    }
	}
      while (optc != -1);

      return true;
    }

  /* Displays the help.  */
  void
  display_help(const char *arg0)
  {
    printf(_("Usage: %s [OPTION]...\n"), arg0);
    printf(_("Display a rotating 3D clock.\n"));
    printf("\n");
    printf(_("  -m, --hide-menu-bar   hide the menu bar\n"));
    printf(_("      --private-colormap allocate a private colormap\n"));
    printf(_("      --help            display this help and exit\n"));
    printf(_("      --version         output version information and exit\n"));
    printf("\n");
    printf(_("Report bugs to <rglclock@lists.hypercore.co.jp>.\n"));
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

  class clock_app
  {
  protected:
    static void edit_options(gpointer, guint, GtkWidget *);
    static void describe(gpointer, guint, GtkWidget *);

  private:
    glclock clock;
    class profile profile;

  public:
    clock_app();
    ~clock_app();

  public:
    GtkWidget *create_window();
  };
} // (unnamed namespace)

void
clock_app::edit_options(gpointer data, guint, GtkWidget *item)
{
  GtkWidget *window = static_cast<GtkWidget *>(data);
  gpointer ud = gtk_object_get_user_data(GTK_OBJECT(window));
  clock_app *d = static_cast<clock_app *>(ud);

  clock_options_dialog dialog(&d->clock);
  dialog.act(GTK_WINDOW(window));

  d->profile.save(&d->clock);
}

void
clock_app::describe(gpointer data, guint, GtkWidget *item)
{
  GtkWidget *window = static_cast<GtkWidget *>(data);

  about_dialog dialog;
  dialog.act(GTK_WINDOW(window));
}

GtkWidget *
clock_app::create_window()
{
  GtkWidget *toplevel = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_object_set_user_data(GTK_OBJECT(toplevel), this);
  gtk_window_set_policy(GTK_WINDOW(toplevel), true, true, false);
  gtk_signal_connect(GTK_OBJECT(toplevel), "delete_event",
		     GTK_SIGNAL_FUNC(gtk_main_quit), this);

  GtkAccelGroup *ag = gtk_accel_group_new();
  gtk_accel_group_attach(ag, GTK_OBJECT(toplevel));

  {
    GtkObject_ptr<GtkWidget> box1(gtk_vbox_new(FALSE, 0));
    {
      GtkObject_ptr<GtkItemFactory> ifactory
	(gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<Window>", ag));
      GtkItemFactoryEntry entries[]
	=
      {
	{_("/_File/_Options..."), NULL,
	 reinterpret_cast<GtkItemFactoryCallback>(&edit_options), 3,
	 "<Item>"},
	{_("/_File/"), NULL, NULL, 0, "<Separator>"},
	{_("/_File/E_xit"), NULL,
	 reinterpret_cast<GtkItemFactoryCallback>(&gtk_main_quit), 1,
	 "<Item>"},
	{_("/_File/"), NULL, NULL, 0, "<Separator>"},
	{_("/_File/_About..."), NULL,
	 reinterpret_cast<GtkItemFactoryCallback>(&describe), 2, "<Item>"}
      };
      gtk_item_factory_create_items(ifactory.get(),
				    sizeof entries / sizeof entries[0],
				    entries, toplevel);

      if (!opt_hide_menu_bar)
	gtk_widget_show(ifactory->widget);
      gtk_box_pack_start(GTK_BOX(box1.get()), ifactory->widget,
			 FALSE, FALSE, 0);

      GtkObject_ptr<GtkWidget> content(clock.create_widget());
      gtk_widget_show(content.get());
      gtk_box_pack_start(GTK_BOX(box1.get()), content.get(),
			 TRUE, TRUE, 0);
      GdkGeometry geometry = {0, 0, 0, 0, 0, 0, 1, 1};
      gtk_window_set_geometry_hints(GTK_WINDOW(toplevel), content.get(),
				    &geometry, GDK_HINT_RESIZE_INC);
    }
    gtk_widget_show(box1.get());
    gtk_container_add(GTK_CONTAINER(toplevel), box1.get());
  }

  return toplevel;
}

clock_app::~clock_app()
{
}

clock_app::clock_app()
{
  string s(getenv("HOME"));
  s.append("/.rglclock");
#ifdef HAVE_MKDIR
  mkdir(s.c_str(), 0777);	// XXX: Ignoring errors.
#endif
  s.append("/options");
  profile.open(s.c_str());
  profile.restore(&clock);
  clock.add_callback(&profile);

  GdkVisual *visual = glclock::best_visual();
  gtk_widget_set_default_visual(visual);

  GdkColormap *cm = gdk_colormap_new(visual, opt_private_colormap);
  gtk_widget_set_default_colormap(cm);
  gdk_colormap_unref(cm);
}

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

  if (opt_help)
    {
      display_help(argv[0]);
      return EXIT_SUCCESS;
    }

  try
    {
      parse_gtkrcs();

      clock_app app;

      GtkWidget *toplevel = 0;
      try
	{
	  toplevel = app.create_window();
	  gtk_widget_show(toplevel);

	  gtk_main();

	  gtk_widget_destroy(toplevel);
	}
      catch (...)
	{
	  if (toplevel != NULL)
	    gtk_widget_destroy(toplevel);
	  throw;
	}
    }
  catch (exception &x)
    {
      fprintf(stderr, _("%s: Unhandled exception: %s\n"), argv[0], x.what());
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
