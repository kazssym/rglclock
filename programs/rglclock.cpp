/*
 * RGLClock - rotating 3D clock
 * Copyright (C) 1998, 1999, 2000, 2002, 2007 Hypercore Software
 * Design, Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <cstdio>

#include "autowidget.h"
#include <gtk/gtk.h>
#include <getopt.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <signal.h>

#if ENABLE_NLS
#include <libintl.h>
#define _(t) gettext (t)
#else
#define _(t) (t)
#endif

#include "clock.h"
#include "profile.h"
#include "about.h"

using std::string;
using std::printf;
using std::putchar;

/* Clock application.  */
class application
{
private:
    glclock clock;
    class profile profile;

    /* Main window of this application.  */
    GtkWidget *window;

public:
    application (void);

public:
    ~application (void);
    GtkWidget *widget (void);

public:
    /* Shows the `Options' dialog.  */
    void show_options_dialog ()
    {
#if 0 /* temporarily disabled */
        clock.show_options_dialog (GTK_WINDOW (main_window));
#endif
    }

    /* Shows the about dialog and returns immediately.  */
    void show_about_dialog ();
};

namespace proxy
{
    /* Handles an `Options' command.  */
    void handle_options_command (gpointer data, guint, GtkWidget *item) throw ()
    {
        application *app = static_cast<application *> (data);

        app->show_options_dialog ();
    }

    /* Handles an `about' command.  */
    void handle_about_command (gpointer data, guint, GtkWidget *item) throw ()
    {
        application *app = static_cast<application *> (data);

        app->show_about_dialog ();
    }
}

application::application (void)
{
    window = NULL;

    string s (getenv ("HOME"));
    s.append ("/.rglclock");
#ifdef HAVE_MKDIR
    mkdir (s.c_str (), 0777);   // XXX: Ignoring errors.
#endif
    s.append ("/options");

    profile.open (s.c_str ());
    profile.restore (&clock);
    clock.add_callback (&profile);

#if 0 /* temporarily disabled */
    GdkVisual *visual = glclock::best_visual ();
    gtk_widget_set_default_visual (visual);

    GdkColormap *cm = gdk_colormap_new (visual, false);
    gtk_widget_set_default_colormap (cm);
    gdk_colormap_unref (cm);
#endif
}

application::~application (void)
{
    // FIXME This seems too late.
    profile.save (&clock);
}

GtkWidget *application::widget (void)
{
    if (window == NULL)
    {
        window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        gtk_window_set_policy (GTK_WINDOW (window), true, true, false);
        gtk_object_set_user_data (GTK_OBJECT (window), this);
        gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                            GTK_SIGNAL_FUNC (gtk_main_quit), this);

        GtkAccelGroup *ag = gtk_accel_group_new ();
        gtk_window_add_accel_group (GTK_WINDOW (window), ag);

        {
            GtkObject_ptr<GtkWidget> box1 (gtk_vbox_new (FALSE, 0));
            gtk_widget_show (box1.get ());
            gtk_container_add (GTK_CONTAINER (window), box1.get ());

            {
                GtkObject_ptr<GtkItemFactory> ifactory
                    (gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<Window>", ag));
#define ITEM_FACTORY_CALLBACK(f) (reinterpret_cast<GtkItemFactoryCallback> (f))
                GtkItemFactoryEntry entries[] = {
                    {_("/_File/_Options..."), NULL,
                     ITEM_FACTORY_CALLBACK (&proxy::handle_options_command), 0, "<Item>"},
                    {_("/_File/"), NULL, NULL, 0, "<Separator>"},
                    {_("/_File/E_xit"), NULL,
                     ITEM_FACTORY_CALLBACK (&gtk_main_quit), 1, "<Item>"},
                    {_("/_Help/_About..."), NULL,
                     ITEM_FACTORY_CALLBACK (&proxy::handle_about_command), 2, "<Item>"}};
#undef ITEM_FACTORY_CALLBACK
                gtk_item_factory_create_items (ifactory.get (),
                                               sizeof entries / sizeof entries[0],
                                               entries, this);

#if 0 /* temporarily disabled */
                if (opt_with_menu_bar)
                    gtk_widget_show (ifactory->widget);
#endif
                gtk_box_pack_start (GTK_BOX (box1.get ()), ifactory->widget,
                                    FALSE, FALSE, 0);

                GtkObject_ptr<GtkWidget> content (clock.widget ());
                gtk_widget_show (content.get());
                gtk_box_pack_start (GTK_BOX (box1.get ()), content.get (),
                                    TRUE, TRUE, 0);
                GdkGeometry geometry = {0, 0, 0, 0, 0, 0, 1, 1};
                gtk_window_set_geometry_hints (GTK_WINDOW (window), content.get (),
                                               &geometry, GDK_HINT_RESIZE_INC);
            }
        }
    }

    return window;
}

void application::show_about_dialog ()
{
    about_dialog dialog (GTK_WINDOW (window));
    gtk_widget_show (dialog.widget ());

    while (GTK_WIDGET_VISIBLE (dialog.widget ()))
    {
        gtk_main_iteration ();
    }
}

/* Command line interface.  */

static int opt_with_menu_bar = true;
static int opt_with_saved_options = true;
static int opt_help;
static int opt_version;

static bool parse_options (int argc, char **argv);
static void display_help (const char *arg0);
static void parse_gtkrcs (void);

int main (int argc, char **argv)
{
#if ENABLE_NLS
    /* Initialize NLS.  */
    textdomain (PACKAGE);
#ifdef LOCALEDIR
    bindtextdomain (PACKAGE, LOCALEDIR);
#endif
#endif

    gtk_set_locale ();
    gtk_init (&argc, &argv);

    if (!parse_options (argc, argv))
    {
        return EXIT_FAILURE;
    }

    if (opt_version)
    {
        printf (_ ("%s version %s\n"
                   "Copyright (C) 1998, 1999, 2000, 2002, 2007 "
                   "Hypercore Software Design, Ltd."),
                PACKAGE_NAME, PACKAGE_VERSION);
        putchar ('\n');
        return EXIT_SUCCESS;
    }

    if (opt_help)
    {
        display_help (argv[0]);
        putchar ('\n');
        return EXIT_SUCCESS;
    }

#if ENABLE_NLS
    /* GTK+ uses UTF-8.  */
    bind_textdomain_codeset (PACKAGE, "UTF-8");
#endif

    parse_gtkrcs ();

    application *app = new application ();
    gtk_widget_show (app->widget ());

    gtk_main ();

    delete app;

    return EXIT_SUCCESS;
}

/* Parses the program options.  */
bool
parse_options (int argc, char **argv)
{
    static const struct option longopts[] = {
        {"hide-menu-bar", no_argument, &opt_with_menu_bar, false},
        {"help", no_argument, &opt_help, 1},
        {"version", no_argument, &opt_version, 1},
        {NULL, 0, NULL, 0}
    };

    int optc;
    do
    {
        int index;
        optc = getopt_long(argc, argv, "m", longopts, &index);

        switch (optc)
        {
        case 'm':
            opt_with_menu_bar = false;
            break;
        case 0:             // long option
            break;
        case '?':
            return false;
        }
    }
    while (optc != -1);

    return true;
}

/* Displays the help.  */
void display_help (const char *arg0)
{
    printf (_ ("Usage: %s [OPTION]...\n"), arg0);
    printf (_ ("Display a 3D rotating clock.\n"));
    printf ("\n");
    printf (_ ("  -m, --hide-menu-bar   hide the menu bar\n"));
    printf (_ ("      --help            display this help and exit\n"));
    printf (_ ("      --version         output version information and exit\n"));
    putchar ('\n');
    printf (_ ("Report bugs to <%s>."), PACKAGE_BUGREPORT);
}

void parse_gtkrcs ()
{
#ifdef PKGDATADIR
    gtk_rc_parse (PKGDATADIR "/gtkrc");
#endif /* PKGDATADIR */

    const char *homedir = getenv ("HOME");
    if (homedir != NULL)
    {
        string gtkrc (homedir);
        gtkrc.append ("/.rglclock");
        gtkrc.append ("/gtkrc");
        gtk_rc_parse (gtkrc.c_str ());
    }
}

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-offsets: ((substatement-open . 0) (arglist-intro . +) (arglist-close . 0))
 * End:
 */
