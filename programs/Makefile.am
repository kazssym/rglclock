## Process this file with automake to produce a Makefile.in.

INCLUDES = -I${top_srcdir}/include \
-DPKGDATADIR=\"${pkgdatadir}\" -DLOCALEDIR=\"${datadir}/locale\" \
-DOPTIONS_DTD=\"${pkgdatadir}/options-0.dtd\" \
${GTK_CFLAGS}

bin_PROGRAMS = rglclock

rglclock_LDFLAGS = -dlopen ../lib/simple.la
rglclock_SOURCES = getopt.c getopt1.c profile.cc main.cc
rglclock_LDADD = ../lib/libclock.a ../lib/libutils.a \
${GL_LIBS} ${GTK_LIBS}

noinst_HEADERS = getopt.h profile.h

pkgdata_DATA = options-0.dtd

man_MANS = rglclock.1

EXTRA_DIST = options-0.dtd ${man_MANS}

