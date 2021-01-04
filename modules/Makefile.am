## Process this file with automake to produce a Makefile.in.

modulesdir = $(pkglibdir)/modules

LIBS =

AM_CPPFLAGS = -I $(top_srcdir)/lib -I $(top_srcdir)/include $(LIBPNG_CFLAGS)

modules_LTLIBRARIES = libplain.la

libplain_la_LDFLAGS = -module -avoid-version
libplain_la_SOURCES = ../lib/simple.c
libplain_la_LIBADD = $(LIBPNG_LIBS)
