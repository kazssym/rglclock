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

#ifdef HAVE_NANA_H
# include <nana.h>
# include <cstdio>
#else
# include <cassert>
# define I assert
#endif

#define _(MSG) gettext(MSG)

using namespace std;

#define YEARS "1998, 1999"

/* Handles a click on the OK button.  */
void
about_dialog::handle_ok(GtkWidget *button)
{
  close(positive_action());
}

namespace
{
  /* Delivers an OK to the options dialog.  */
  void
  deliver_ok(GtkWidget *w, gpointer data) throw ()
  {
    about_dialog *recipient = static_cast<about_dialog *>(data);
    I(recipient != NULL);

    recipient->handle_ok(w);
  }
} // (unnamed)

/* Populates a dialog with children.  */
void
about_dialog::populate(GtkDialog *dialog)
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
  GtkObject_ptr<GtkWidget> label1(gtk_label_new(version));
  free(version);
  gtk_label_set_justify(GTK_LABEL(label1.get()), GTK_JUSTIFY_LEFT);
  gtk_misc_set_padding(GTK_MISC(label1.get()), 10, 10);
  gtk_widget_show(label1.get());
  gtk_box_pack_start(GTK_BOX(dialog->vbox), label1.get(),
		     FALSE, FALSE, 0);

  /* Makes the OK button.  */
  const char *ok_text = _("OK");
  GtkObject_ptr<GtkWidget> ok_button(gtk_button_new_with_label(ok_text));
  gtk_signal_connect(GTK_OBJECT(ok_button.get()), "clicked",
		     GTK_SIGNAL_FUNC(&deliver_ok), this);
  gtk_widget_show(ok_button.get());
  gtk_box_pack_start(GTK_BOX(dialog->action_area), ok_button.get(),
		     FALSE, FALSE, 0);
  gtk_window_set_focus(GTK_WINDOW(dialog), ok_button.get());
}

void
about_dialog::configure(GtkDialog *dialog)
{
  /* Window title.  */
  const char *title_format = _("About %s");
  /* I decided to avoid glib functions here, as the format string may
     contain extended conversions like "%1$s".  */
  char *title;
#ifdef HAVE_ASPRINTF
  asprintf(&title, title_format, PACKAGE);
#else /* not HAVE_ASPRINTF */
  title = static_cast<char *>(malloc(strlen(title_format) + sizeof PACKAGE));
  sprintf(title, title_format, PACKAGE);
#endif /* not HAVE_ASPRINTF */
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  free(title);

  gtk_window_set_policy(GTK_WINDOW(dialog), FALSE, FALSE, FALSE);
  gtk_window_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

  populate(dialog);
}

