## Process this file with automake to produce a Makefile.in.

INCLUDES = -I${top_srcdir}/include \
-DPKGDATADIR=\"${pkgdatadir}\" -DLOCALEDIR=\"${datadir}/locale\" \
${GTK_CFLAGS}

bin_PROGRAMS = rglclock

rglclock_LDFLAGS = -dlopen ../lib/simple.la
rglclock_SOURCES = getopt.c getopt1.c profile.cc main.cc
rglclock_LDADD = ../lib/libclock.a \
../lib/libutils.a ../lib/libgdkgl.a \
${GL_LIBS} ${GTK_LIBS}

noinst_HEADERS = getopt.h profile.h

man_MANS = rglclock.1

EXTRA_DIST = ${man_MANS}

