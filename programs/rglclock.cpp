/*
 * RGLClock - 3D rotating clock
 * Copyright (C) 1998, 2000, 2007 Hypercore Software Design, Ltd.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "profile.h"
#include "glclock.h"

#include "autowidget.h"
#include <gtk/gtk.h>
#include <getopt.h>
#include <libintl.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <signal.h>

#include <cstdio>
#include <string>
#include <exception>

#define _(MSG) gettext (MSG)

using namespace std;

#define PROGRAM "rglclock"

#define COPYRIGHT_YEAR "1998, 2000"

/* Clock application.  */
class clock_app
{
public:
    static int opt_with_menu_bar;
    static int opt_with_saved_options;
    static int opt_with_private_colormap;

private:
    glclock clock;
    class profile profile;

    /* Main window of this application.  */
    GtkWidget *main_window;

public:
    clock_app();
    ~clock_app();

public:
    GtkWidget *create_window();

public:
    /* Shows the `Options' dialog.  */
    void show_options_dialog()
    {clock.show_options_dialog(GTK_WINDOW(main_window));}

    /* Shows the about dialog and returns immediately.  */
    void show_about_dialog();
};

int clock_app::opt_with_menu_bar = true;
int clock_app::opt_with_saved_options = true;
int clock_app::opt_with_private_colormap = false;

/* Window management.  */

namespace proxy
{
    void handle_about_ok_clicked (GtkButton *button, gpointer data) throw ()
    {
        GtkWidget *dialog = gtk_widget_get_toplevel (GTK_WIDGET (button));

        gtk_widget_destroy (dialog);
    }
}

void clock_app::show_about_dialog ()
{
    GtkWidget *dialog = gtk_dialog_new ();

    try
    {
        gtk_window_set_policy(GTK_WINDOW(dialog), false, false, false);
        gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
        gtk_window_set_modal(GTK_WINDOW(dialog), true);
        if (main_window != NULL)
            gtk_window_set_transient_for(GTK_WINDOW(dialog),
                                         GTK_WINDOW(main_window));

        const char *title_format = _("About %s");
        char *title;
#ifdef HAVE_ASPRINTF
        asprintf(&title, title_format, PROGRAM);
#else
        title = static_cast<char *>(malloc(strlen(title_format) - 2
                                           + strlen(PROGRAM) + 1));
        sprintf(title, title_format, PROGRAM);
#endif

        gtk_window_set_title(GTK_WINDOW(dialog), title);
        free(title);

        GtkWidget *vbox1 = gtk_vbox_new(false, 10);
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vbox1,
                           false, false, 0);

        gtk_widget_show(vbox1);
        gtk_container_set_border_width(GTK_CONTAINER(vbox1), 20);
        {
            const char *version_format = "%s %s";
            char *version;
#ifdef HAVE_ASPRINTF
            asprintf(&version, version_format, PROGRAM, VERSION);
#else
            version = static_cast<char *>(malloc(strlen(version_format) - 2 * 2
                                                 + strlen(PROGRAM)
                                                 + strlen(VERSION) + 1));
            sprintf(version, version_format, PROGRAM, VERSION);
#endif

            GtkWidget *version_label = gtk_label_new(version);
            gtk_box_pack_start(GTK_BOX(vbox1), version_label, false, false, 0);

            gtk_widget_show(version_label);
            gtk_misc_set_alignment(GTK_MISC(version_label), 0.0, 0.5);
            gtk_label_set_justify(GTK_LABEL(version_label), GTK_JUSTIFY_LEFT);
            free(version);

            const char *copyright_format
                = _("Copyright (C) %s Hypercore Software Design, Ltd.");
            char *copyright;
#ifdef HAVE_ASPRINTF
            asprintf(&copyright, copyright_format, COPYRIGHT_YEAR);
#else
            copyright = static_cast<char *>(malloc(strlen(copyright_format) - 2
                                                   + strlen(COPYRIGHT_YEAR) + 1));
            sprintf(copyright, copyright_format, COPYRIGHT_YEAR);
#endif

            GtkWidget *copyright_label = gtk_label_new(copyright);
            gtk_box_pack_start(GTK_BOX(vbox1), copyright_label, false, false, 0);

            gtk_widget_show(copyright_label);
            gtk_misc_set_alignment(GTK_MISC(copyright_label), 0.0, 0.5);
            gtk_label_set_justify(GTK_LABEL(copyright_label), GTK_JUSTIFY_LEFT);
            free(copyright);

            const char *notice1
                = _("This is free software; see the source for copying conditions.\n"
                    "There is NO WARRANTY; not even for MERCHANTABILITY\n"
                    "or FITNESS FOR A PARTICULAR PURPOSE.");
            GtkWidget *notice1_label = gtk_label_new(notice1);
            gtk_box_pack_start(GTK_BOX(vbox1), notice1_label, false, false, 0);

            gtk_widget_show(notice1_label);
            gtk_misc_set_alignment(GTK_MISC(notice1_label), 0.0, 0.5);
            gtk_label_set_justify(GTK_LABEL(notice1_label), GTK_JUSTIFY_LEFT);
            // gtk_label_set_line_wrap(GTK_LABEL(notice1_label), true);

            const char *report_bugs
                = _("Send comments or report bugs to <rglclock@lists.hypercore.co.jp>.");
            GtkWidget *report_bugs_label = gtk_label_new(report_bugs);
            gtk_box_pack_start(GTK_BOX(vbox1), report_bugs_label, false, false, 0);

            gtk_widget_show(report_bugs_label);
            gtk_misc_set_alignment(GTK_MISC(report_bugs_label), 0.0, 0.5);
            gtk_label_set_justify(GTK_LABEL(report_bugs_label), GTK_JUSTIFY_LEFT);
            gtk_label_set_line_wrap(GTK_LABEL(report_bugs_label), true);
        }

        const char *ok = _("OK");
        GtkWidget *ok_button = gtk_button_new_with_label(ok);
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
                           ok_button, false, false, 0);

        gtk_widget_show(ok_button);
        gtk_window_set_focus(GTK_WINDOW(dialog), ok_button);
        gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
                           GTK_SIGNAL_FUNC(&handle_about_ok_clicked), this);
    }
    catch (...)
    {
        gtk_widget_destroy(dialog);
        throw;
    }

    gtk_widget_show(dialog);
}

namespace proxy
{
    /* Handles an `Options' command.  */
    void handle_options_command (gpointer data, guint, GtkWidget *item) throw ()
    {
        clock_app *app = static_cast<clock_app *> (data);

        app->show_options_dialog ();
    }

    /* Handles an `about' command.  */
    void handle_about_command (gpointer data, guint, GtkWidget *item) throw ()
    {
        clock_app *app = static_cast<clock_app *> (data);

        app->show_about_dialog ();
    }
}

GtkWidget *clock_app::create_window ()
{
    if (main_window == NULL)
    {
        main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

        gtk_object_set_user_data (GTK_OBJECT (main_window), this);
        gtk_window_set_policy (GTK_WINDOW (main_window), true, true, false);
        gtk_signal_connect (GTK_OBJECT (main_window), "delete_event",
                            GTK_SIGNAL_FUNC (gtk_main_quit), this);

        GtkAccelGroup *ag = gtk_accel_group_new ();
        gtk_accel_group_attach (ag, GTK_OBJECT (main_window));

        {
            GtkObject_ptr<GtkWidget> box1 (gtk_vbox_new (FALSE, 0));
            gtk_widget_show (box1.get ());
            gtk_container_add (GTK_CONTAINER (main_window), box1.get ());

            {
                GtkObject_ptr<GtkItemFactory> ifactory
                    (gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<Window>", ag));
#define ITEM_FACTORY_CALLBACK(f) (reinterpret_cast<GtkItemFactoryCallback> (f))
                GtkItemFactoryEntry entries[] = {
                    {_("/_File/_Options..."), NULL,
                     ITEM_FACTORY_CALLBACK (&handle_options_command), 0, "<Item>"},
                    {_("/_File/"), NULL, NULL, 0, "<Separator>"},
                    {_("/_File/E_xit"), NULL,
                     ITEM_FACTORY_CALLBACK (&gtk_main_quit), 1, "<Item>"},
                    {_("/_Help/_About..."), NULL,
                     ITEM_FACTORY_CALLBACK (&handle_about_command), 2, "<Item>"}};
#undef ITEM_FACTORY_CALLBACK
                gtk_item_factory_create_items (ifactory.get (),
                                               sizeof entries / sizeof entries[0],
                                               entries, this);

                if (opt_with_menu_bar)
                    gtk_widget_show (ifactory->widget);
                gtk_box_pack_start (GTK_BOX (box1.get ()), ifactory->widget,
                                    FALSE, FALSE, 0);

                GtkObject_ptr<GtkWidget> content (clock.create_widget());
                gtk_widget_show (content.get());
                gtk_box_pack_start (GTK_BOX (box1.get ()), content.get (),
                                    TRUE, TRUE, 0);
                GdkGeometry geometry = {0, 0, 0, 0, 0, 0, 1, 1};
                gtk_window_set_geometry_hints (GTK_WINDOW (main_window), content.get (),
                                               &geometry, GDK_HINT_RESIZE_INC);
            }
        }
    }

    return main_window;
}

clock_app::~clock_app ()
{
    // FIXME This seems too late.
    profile.save (&clock);
}

clock_app::clock_app ()
    : main_window (NULL)
{
    string s (getenv ("HOME"));
    s.append ("/.rglclock");
#ifdef HAVE_MKDIR
    mkdir (s.c_str (), 0777);   // XXX: Ignoring errors.
#endif
    s.append ("/options");

    profile.open (s.c_str ());
    if (opt_with_saved_options)
        profile.restore (&clock);
    clock.add_callback (&profile);

    GdkVisual *visual = glclock::best_visual ();
    gtk_widget_set_default_visual (visual);

    GdkColormap *cm = gdk_colormap_new (visual, opt_with_private_colormap);
    gtk_widget_set_default_colormap (cm);
    gdk_colormap_unref (cm);
}

/* Command line interface.  */

namespace
{
    int opt_help = 0;
    int opt_version = 0;

    /* Parses the program options.  */
    bool
    parse_options (int argc, char **argv)
    {
        static struct option const longopts[] = {
            {"hide-menu-bar", no_argument, &clock_app::opt_with_menu_bar, false},
            {"no-options", no_argument, &clock_app::opt_with_saved_options, false},
            {"private-colormap", no_argument,
             &clock_app::opt_with_private_colormap, true},
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
                clock_app::opt_with_menu_bar = false;
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
        printf (_ ("      --private-colormap allocate a private colormap\n"));
        printf (_ ("      --no-options      start without restoring options\n"));
        printf (_ ("      --help            display this help and exit\n"));
        printf (_ ("      --version         output version information and exit\n"));
        printf ("\n");
        printf (_ ("Report bugs to <%s>.\n"), PACKAGE_BUGREPORT);
    }

    void parse_gtkrcs ()
    {
#ifdef PKGDATADIR
        gtk_rc_parse (PKGDATADIR "/gtkrc");
#endif /* PKGDATADIR */

        char const *homedir = getenv ("HOME");
        if (homedir != NULL)
        {
            string gtkrc (homedir);
            gtkrc.append ("/.rglclock");
            gtkrc.append ("/gtkrc");
            gtk_rc_parse (gtkrc.c_str ());
        }
    }
} // (unnamed)

int main (int argc, char **argv)
{
    gtk_set_locale ();
    gtk_init (&argc, &argv);

    if (!parse_options (argc, argv))
    {
        return EXIT_FAILURE;
    }

    /* Initialize NLS.  */
#ifdef LOCALEDIR
    bindtextdomain (PACKAGE, LOCALEDIR);
#endif
    textdomain (PACKAGE);

    if (opt_version)
    {
        printf ("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
        return EXIT_SUCCESS;
    }

    if (opt_help)
    {
        display_help (argv[0]);
        return EXIT_SUCCESS;
    }

    parse_gtkrcs ();

    clock_app app;

    GtkWidget *toplevel = app.create_window ();
    try
    {
        gtk_widget_show (toplevel);

        gtk_main ();
    }
    catch (...)
    {
        gtk_widget_destroy (toplevel);
        throw;
    }
    gtk_widget_destroy (toplevel);

    return EXIT_SUCCESS;
}

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-offsets: ((substatement-open . 0))
 * End:
 */
