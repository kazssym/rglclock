## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I$(top_srcdir)/libgnu -I$(top_builddir)/libgnu \
$(LIBPNG_CFLAGS) $(GTK_CFLAGS)

lib_LTLIBRARIES = librglclock.la

include_HEADERS = rglclockmod.h

noinst_HEADERS = clock.h simple.h g_ptr.h utils.h mat4.h

librglclock_la_LDFLAGS = -no-undefined -version-info 1:0:0
librglclock_la_SOURCES = movement.cpp mat4.c ../modules/simple.c
librglclock_la_LIBADD = $(LIBPNG_LIBS) $(GTK_LIBS)
