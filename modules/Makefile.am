## Process this file with automake to produce a Makefile.in.

modulesdir = $(pkglibdir)/modules

INCLUDES = -I../include -I../lib

LIBS =

modules_LTLIBRARIES = simple.la

simple_la_LDFLAGS = -module -avoid-version
simple_la_SOURCES = simple.c
simple_la_LIBADD = $(GL_LIBS) $(PNG_LIBS)
