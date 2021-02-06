## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I $(top_srcdir)/include -I $(top_srcdir)/programs \
-I$(top_srcdir)/libgnu -I$(top_builddir)/libgnu \
$(LIBPNG_CFLAGS) $(GTK_CFLAGS)

noinst_LIBRARIES = libsimple.a
lib_LTLIBRARIES = librglclock.la

noinst_HEADERS = clock.h simple.h

libsimple_a_SOURCES = simple.c

librglclock_la_LDFLAGS = -release 2 -version-info 0:0:0
librglclock_la_SOURCES = module.cpp clockoptions.cc
librglclock_la_LIBADD = $(GTK_LIBS)

