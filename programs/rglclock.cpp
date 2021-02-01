// rglclock.cpp
// Copyright (C) 1998-2007 Hypercore Software Design, Ltd.
// Copyright (C) 2021 Kaz Nishimura
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <cstdio>

#include "g_ptr.h"
#include <gtk/gtk.h>
#include <gettext.h>
#include <getopt.h>
#include <sys/stat.h>
#include <signal.h>

#include "clock.h"
#include "profile.h"
#include "about.h"

using std::string;
using std::printf;
using std::putchar;

#define _(String) gettext(String)
#define N_(String) gettext_noop(String)

/* Clock application.  */
class application
{
private:
    glclock clock;

private:
    profile _profile;

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
    mkdir (s.c_str (), 0777);   // XXX: Ignoring errors.
    s.append ("/options");

    _profile.open (s.c_str ());
    _profile.restore (&clock);
    clock.add_callback (&_profile);

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
    _profile.save (&clock);
}

GtkWidget *application::widget (void)
{
    if (window == NULL)
    {
        window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        // gtk_window_set_policy (GTK_WINDOW (window), true, true, false);
        g_signal_connect(G_OBJECT(window), "delete_event",
                         G_CALLBACK(gtk_main_quit), this);

        GtkAccelGroup *ag = gtk_accel_group_new ();
        gtk_window_add_accel_group (GTK_WINDOW (window), ag);

        auto &&content = g_ptr<GtkWidget>(clock.widget());
        gtk_widget_show(&*content);
        gtk_container_add(GTK_CONTAINER(window), &*content);

        GdkGeometry geometry {0, 0, 0, 0, 0, 0, 1, 1};
        gtk_window_set_geometry_hints(GTK_WINDOW(window), &*content,
            &geometry, GDK_HINT_RESIZE_INC);
    }

    return window;
}

void application::show_about_dialog ()
{
    about_dialog dialog (GTK_WINDOW (window));
    gtk_widget_show (dialog.widget ());

    while (gtk_widget_get_visible(dialog.widget())) {
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
