## Process this file with automake to produce a Makefile.in.

INCLUDES = -I${top_srcdir}/include \
${GTK_CFLAGS}

LIBS =

pkglib_LTLIBRARIES = simple.la
noinst_LIBRARIES = libgdkgl.a libutils.a libclock.a

libgdkgl_a_SOURCES = gdkgl.c

libutils_a_SOURCES = modaldialog.cc options.cc

simple_la_LDFLAGS = -module -avoid-version
simple_la_SOURCES = simple.c
simple_la_LIBADD = ${GL_LIBS}

libclock_a_SOURCES = module.cc glclock.cc clockoptions.cc

noinst_HEADERS = simple.h

