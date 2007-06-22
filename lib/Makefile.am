## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I $(top_srcdir)/include -I $(top_srcdir)/programs \
	$(GTK_CFLAGS)

noinst_LIBRARIES = libsimple.a librglclock.a

libsimple_a_SOURCES = simple.c

librglclock_a_SOURCES = module.cpp clockoptions.cc

noinst_HEADERS = simple.h

