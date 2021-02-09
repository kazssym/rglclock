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


#include "g_ptr.h"
#include <gtk/gtk.h>
#include <gettext.h>
#include <getopt.h>
#include <sys/stat.h>
#include <signal.h>
#include <locale>
#include <string>
#include <cstdio>

#include "clock.h"
#include "profile.h"
#include "about.h"

using std::fprintf;
using std::locale;
using std::printf;
using std::putchar;
using std::runtime_error;
using std::string;

#define _(String) gettext(String)
#define N_(String) gettext_noop(String)

extern "C" void handle_activate(GApplication *app, gpointer data) noexcept;
extern "C" void handle_app_exit(GSimpleAction *action, GVariant *parameter,
    gpointer data) noexcept;
extern "C" void handle_app_about(GAction *action, GVariant *parameter,
    gpointer data) noexcept;

/* Clock application.  */
class rglclock_app
{
    friend void handle_app_exit(GSimpleAction *action, GVariant *parameter,
        gpointer data) noexcept;

private:

    g_ptr<GtkApplication> _app;

    movement _clock;

    profile _profile;

public:

    // Constructors.

    explicit rglclock_app(const g_ptr<GtkApplication> &app);

    rglclock_app(const rglclock_app &) = delete;


    // Destructor.

    ~rglclock_app();


    // Assignment operators.

    void operator =(const rglclock_app &) = delete;


    void start();

    /* Shows the `Options' dialog.  */
    void show_options_dialog()
    {
#if 0 /* temporarily disabled */
        _clock.show_options_dialog (GTK_WINDOW (main_window));
#endif
    }

    /* Shows the about dialog and returns immediately.  */
    void show_about_dialog();
};

void handle_activate(GApplication *, gpointer data) noexcept
{
    auto &&app = static_cast<rglclock_app *>(data);
    app->start();
}

void handle_app_exit(GSimpleAction *, GVariant *, gpointer data) noexcept
{
    auto &&app = static_cast<rglclock_app *>(data);
    g_application_quit(G_APPLICATION(&*(app->_app)));
}

void handle_app_about(GAction *, GVariant *, gpointer data) noexcept
{
    auto &&clock = static_cast<rglclock_app *>(data);
    clock->show_about_dialog();
}

rglclock_app::rglclock_app(const g_ptr<GtkApplication> &app):
    _app {app}
{
    string s (getenv ("HOME"));
    s.append ("/.rglclock");
    mkdir (s.c_str (), 0777);   // XXX: Ignoring errors.
    s.append ("/options");

    _profile.open (s.c_str ());
    _profile.restore (&_clock);

    g_signal_connect(&*_app, "activate", G_CALLBACK(handle_activate), this);

    auto &&exit = g_simple_action_new("exit", nullptr);
    g_signal_connect(exit, "activate", G_CALLBACK(handle_app_exit), this);
    g_action_map_add_action(G_ACTION_MAP(&*_app), G_ACTION(exit));

    auto &&about = g_simple_action_new("about", nullptr);
    g_signal_connect(about, "activate", G_CALLBACK(handle_app_about), this);
    g_action_map_add_action(G_ACTION_MAP(&*_app), G_ACTION(about));

#if 0 /* temporarily disabled */
    GdkVisual *visual = movement::best_visual ();
    gtk_widget_set_default_visual (visual);

    GdkColormap *cm = gdk_colormap_new (visual, false);
    gtk_widget_set_default_colormap (cm);
    gdk_colormap_unref (cm);
#endif
}

rglclock_app::~rglclock_app (void)
{
    // FIXME This seems too late.
    _profile.save (&_clock);
}

void rglclock_app::start()
{
    g_ptr<GtkWidget> window {gtk_application_window_new(&*_app)};
    // g_signal_connect(&*window, "delete_event",
    //     G_CALLBACK(gtk_main_quit), this);

    auto &&content = _clock.widget();
    gtk_widget_show(&*content);
    gtk_container_add(GTK_CONTAINER(&*window), &*content);

    gtk_widget_show(&*window);
}

void rglclock_app::show_about_dialog()
{
    about_dialog dialog;
    dialog.show_modal();
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
    try {
        locale::global(locale(""));
    }
    catch (const runtime_error &error) {
        fprintf(stderr, "error: failed to set locale: %s\n", error.what());
    }

#if ENABLE_NLS
    /* Initialize NLS.  */
    textdomain(PACKAGE_TARNAME);
#ifdef LOCALEDIR
    bindtextdomain(PACKAGE_TARNAME, LOCALEDIR);
#endif
#endif

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
    bind_textdomain_codeset(PACKAGE_TARNAME, "UTF-8");
#endif

    parse_gtkrcs ();

    g_ptr<GtkApplication> app {
        gtk_application_new(nullptr, G_APPLICATION_FLAGS_NONE)};

    rglclock_app rglclock {app};

    return g_application_run(G_APPLICATION(&*app), 0, nullptr);
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
