## Process this file with automake to produce a Makefile.in.

INCLUDES = ${GTK_CFLAGS}

bin_PROGRAMS = rglclock

rglclock_SOURCES = main.cc glclock.cc
rglclock_LDADD = gdkGL/libgdkGL.a ${GL_LIBS} ${GTK_LIBS}

noinst_HEADERS = glclock.h

SUBDIRS = gdkGL
