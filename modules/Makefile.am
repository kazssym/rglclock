## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I $(top_srcdir)/lib -I $(top_srcdir)/include \
$(LIBPNG_CFLAGS)

modulesdir = $(pkglibdir)/modules

modules_LTLIBRARIES = simple.la

simple_la_LDFLAGS = -module -avoid-version
simple_la_SOURCES = simple.c
simple_la_LIBADD = $(LIBPNG_LIBS)
