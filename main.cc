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
