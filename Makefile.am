## Process this file with automake to produce a Makefile.in.

INCLUDES = -DPKGDATADIR=\"${pkgdatadir}\" -DLOCALEDIR=\"${datadir}/locale\" \
${GTK_CFLAGS}

noinst_LIBRARIES = libgdkgl.a libclock.a
bin_PROGRAMS = rglclock

libgdkgl_a_SOURCES = gdkgl.c

libclock_a_SOURCES = simple.c module.cc glclock.cc \
options.cc clockoptions.cc about.cc

rglclock_SOURCES = getopt.c getopt1.c profile.cc main.cc
rglclock_LDADD = libclock.a libgdkgl.a ${GL_LIBS} ${GTK_LIBS}

noinst_HEADERS = utils.h profile.h glclock.h module.h simple.h autowidget.h gdkgl.h \
getopt.h

man_MANS = rglclock.1

EXTRA_DIST = README.ja MEMO.ja VERSION-ID ${man_MANS}

SUBDIRS = intl po

update-po:
	cd po && ${MAKE} update-po

dist-snapshot distcheck-snapshot:
	${MAKE} VERSION=${VERSION}x`date +%Y%m%d` `echo $@ | sed -e 's/-snapshot//'`

