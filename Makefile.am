## Process this file with automake to produce a Makefile.in.

INCLUDES = ${GTK_CFLAGS} -DSESSION

bin_PROGRAMS = rglclock

rglclock_SOURCES = main.cc glclock.cc module.cc gdkgl.c
rglclock_LDADD = ${GL_LIBS} ${GTK_LIBS}

noinst_HEADERS = glclock.h module.h gdkgl.h

man_MANS = rglclock.1

EXTRA_DIST = ${man_MANS}

