## Process this file with automake to produce a Makefile.in.

INCLUDES = -I${top_srcdir}/include \
${GTK_CFLAGS}

LIBS =

pkglib_LTLIBRARIES = simple.la
noinst_LIBRARIES = libutils.a libclock.a

libutils_a_SOURCES = modaldialog.cc options.cc gdk_gl.cc

simple_la_LDFLAGS = -module -avoid-version
simple_la_SOURCES = simple.c
simple_la_LIBADD = ${GL_LIBS}

libclock_a_SOURCES = module.cc glclock.cc clockoptions.cc

noinst_HEADERS = gdk_gl.h simple.h

