## Process this file with automake to produce a Makefile.in.

INCLUDES = -I${top_srcdir}/include \
${GTK_CFLAGS}

noinst_LIBRARIES = libutils.a libsimple.a libclock.a

libutils_a_SOURCES = modaldialog.cc options.cc gdk_gl.cc

libsimple_a_SOURCES = simple.c

libclock_a_SOURCES = module.cc glclock.cc clockoptions.cc

noinst_HEADERS = gdk_gl.h simple.h

