/* -*-C++-*- */

#ifndef glclock_h
#define glclock_h 1

#include <time.h>
#include <gtk/gtkwidget.h>
#include <gdk/gdktypes.h>
#include "gdkGL/gdkGLtypes.h"

class glclock
{
public:
  glclock ();
  ~glclock ();
public:
  operator GtkWidget *() const;
protected:
  static void create_context (GtkWidget *, gpointer);
  static gint handle_expose_event (GtkWidget *, GdkEventExpose *, gpointer);
  static void update (gpointer);
private:
  GtkWidget *drawing_area;
  GdkGLContext *context;
  struct tm tm;
  guint timeout_id;
};

#endif /* not glclock_h */
