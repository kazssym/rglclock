## Process this file with automake to produce a Makefile.in.

INCLUDES = -I../include \
$(GTK_CFLAGS)

noinst_LIBRARIES = libutils.a libclock.a

libutils_a_SOURCES = modaldialog.cc options.cc gdk_gl.cc

libclock_a_SOURCES = module.cc glclock.cc clockoptions.cc

noinst_HEADERS = gdk_gl.h simple.h

