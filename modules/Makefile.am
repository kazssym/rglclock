## Process this file with automake to produce a Makefile.in.

modulesdir = $(pkglibdir)/modules

LIBS =

AM_CPPFLAGS = -I $(top_srcdir)/lib -I $(top_srcdir)/include

modules_LTLIBRARIES = plain.la

plain_la_LDFLAGS = -module -avoid-version

plain_la_SOURCES = ../lib/simple.c
plain_la_LIBADD = $(PNG_LIBS)
