/* rglclock - Rotating GL Clock.
   Copyright (C) 1999 Hypercore Software Design, Ltd.

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
#undef const

#include "glclock.h"

#include "autowidget.h"
#include <gtk/gtk.h>
#include <libintl.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#ifdef ENABLE_TRANSIENT_FOR_HINT
# include <gdk/gdkx.h>
#endif

#define _(MSG) gettext(MSG)

using namespace std;

/* Shows this about dialog and wait.  The parent widget will become
   insensible while this dialog is shown.  */
void
about_dialog::show()
{
  gtk_widget_show(dialog);

  gtk_grab_add(dialog);
  gtk_main();
  gtk_grab_remove(dialog);

  gtk_widget_hide(dialog);
}

about_dialog::~about_dialog()
{
  gtk_widget_destroy(dialog);
}

about_dialog::about_dialog(GtkWidget *parent)
  : parent_widget(parent),
    dialog(NULL)
{
  auto_widget tmp(gtk_dialog_new());
  dialog = tmp.get();

  /* Sets the window title.  */
  const char *title_format = _("About %s");
  /* I decided to avoid glib functions here, as the format string may
     contain extended conversions like "%1$s".  */
  char *title;
#ifdef HAVE_ASPRINTF
  asprintf(&title, title_format, PACKAGE);
#else /* not HAVE_ASPRINTF */
  title = (char *) malloc(strlen(title_format) + sizeof PACKAGE);
  sprintf(title, title_format, PACKAGE);
#endif /* not HAVE_ASPRINTF */
  gtk_window_set_title(GTK_WINDOW(tmp.get()), title);
  free(title);

  gtk_window_set_policy(GTK_WINDOW(tmp.get()), FALSE, FALSE, FALSE);
  gtk_window_position(GTK_WINDOW(tmp.get()), GTK_WIN_POS_CENTER);
  gtk_signal_connect(GTK_OBJECT(tmp.get()), "delete_event",
		     GTK_SIGNAL_FUNC(handle_delete_event), this);
  gtk_signal_connect_after(GTK_OBJECT(tmp.get()), "realize",
			   GTK_SIGNAL_FUNC(finish_realize), this);

  dialog = tmp.get();
  populate(dialog);

  gtk_widget_ref(dialog);
  gtk_object_sink(GTK_OBJECT(dialog));
}

#define YEARS "1998, 1999"

/* Populates a dialog with children.  */
void
about_dialog::populate(GtkWidget *dialog)
{
  /* Makes the version label.  */
  const char *version_format
    = _("%s %s\nCopyright (C) %s Hypercore Software Design, Ltd.");
  char *version;
  /* I decided to avoid glib functions here, as the format string may
     contain extended conversions like "%1$s".  */
#ifdef HAVE_ASPRINTF
  asprintf(&version, version_format, PACKAGE, VERSION, YEARS);
#else /* not HAVE_ASPRINTF */
  version = (char *) malloc(strlen(version_format)
			    + sizeof PACKAGE + sizeof VERSION + sizeof YEARS);
  sprintf(version, version_format, PACKAGE, VERSION, YEARS);
#endif /* not HAVE_ASPRINTF */
  auto_widget label1(gtk_label_new(version));
  free(version);
  gtk_label_set_justify(GTK_LABEL(label1.get()), GTK_JUSTIFY_LEFT);
  gtk_misc_set_padding(GTK_MISC(label1.get()), 10, 10);
  gtk_widget_show(label1.get());
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label1.get(),
		     FALSE, FALSE, 0);

  /* Makes the OK button.  */
  const char *ok_text = _("OK");
  auto_widget ok_button(gtk_button_new_with_label(ok_text));
  gtk_signal_connect(GTK_OBJECT(ok_button.get()), "clicked",
		     GTK_SIGNAL_FUNC(handle_ok), dialog);
  gtk_widget_show(ok_button.get());
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), ok_button.get(),
		     FALSE, FALSE, 0);
  gtk_window_set_default(GTK_WINDOW(dialog), ok_button.get());
}

/* Handles a click on the OK button.  */
void
about_dialog::handle_ok(GtkWidget *dialog,
			gpointer data)
{
  gtk_main_quit();
}

/* Handles a delete event on the dialog.  */
gint
about_dialog::handle_delete_event(GtkWidget *dialog,
				  GdkEventAny *event,
				  gpointer data)
{
  gtk_main_quit();
  return 1;
}

/* Finish the realize process.  */
void
about_dialog::finish_realize(GtkWidget *dialog,
			     gpointer data)
{
  about_dialog *about = static_cast <about_dialog *> (data);

#ifdef ENABLE_TRANSIENT_FOR_HINT
  if (about->parent_widget != NULL)
    {
      GdkWindow *w = dialog->window;
      XSetTransientForHint(GDK_WINDOW_XDISPLAY(w), GDK_WINDOW_XWINDOW(w),
			   GDK_WINDOW_XWINDOW(about->parent_widget->window));
    }
#endif /* ENABLE_TRANSIENT_FOR_HINT */
}

