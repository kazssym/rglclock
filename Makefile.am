## Process this file with automake to produce a Makefile.in.

INCLUDES = -DPKGDATADIR=\"${pkgdatadir}\" -DLOCALEDIR=\"${datadir}/locale\" \
${GTK_CFLAGS}

bin_PROGRAMS = rglclock

rglclock_SOURCES = main.cc glclock.cc about.cc module.cc simple.c gdkgl.c
rglclock_LDADD = ${GL_LIBS} ${GTK_LIBS}

noinst_HEADERS = glclock.h module.h simple.h gdkgl.h
pkgdata_DATA = gtkrc

man_MANS = rglclock.1

EXTRA_DIST = gtkrc ${man_MANS}

SUBDIRS = intl po

dist-snapshot:
	${MAKE} VERSION=${VERSION}.`date +%Y%m%d` dist

