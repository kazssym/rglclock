## Process this file with automake to produce a Makefile.in.

INCLUDES = -DPKGDATADIR=\"${pkgdatadir}\" -DLOCALEDIR=\"${datadir}/locale\" \
${GTK_CFLAGS}

noinst_LIBRARIES = libgdkgl.a libclock.a
bin_PROGRAMS = rglclock

libgdkgl_a_SOURCES = gdkgl.c

libclock_a_SOURCES = simple.c module.cc options.cc

rglclock_SOURCES = clockoptions.cc glclock.cc about.cc main.cc
rglclock_LDADD = libclock.a libgdkgl.a ${GL_LIBS} ${GTK_LIBS}

noinst_HEADERS = options.h glclock.h module.h simple.h autowidget.h gdkgl.h
pkgdata_DATA = gtkrc

man_MANS = rglclock.1

EXTRA_DIST = README.ja VERSION-ID gtkrc ${man_MANS}

SUBDIRS = intl po

update-po:
	cd po && ${MAKE} update-po

dist-snapshot:
	${MAKE} VERSION=${VERSION}x.`date +%Y%m%d` dist

