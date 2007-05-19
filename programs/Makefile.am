## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I ../libgnu -I $(top_srcdir)/libgnu -I $(top_srcdir)/include \
	-DPKGDATADIR=\"${pkgdatadir}\" -DLOCALEDIR=\"${datadir}/locale\" \
	-DOPTIONS_DTD=\"${pkgdatadir}/options-0.dtd\" \
	$(GTK_CFLAGS) $(LTDL_CPPFLAGS)

bin_PROGRAMS = rglclock

rglclock_SOURCES = rglclock.cpp profile.cpp
rglclock_LDADD = ../lib/libclock.a ../lib/libsimple.a \
	../lib/libutils.a ../libgnu/libgnu.la \
	$(GL_LIBS) $(GTK_LIBS) $(LTDL_LIBS) $(PNG_LIBS)

noinst_HEADERS = profile.h

pkgdata_DATA = options-0.dtd

man_MANS = rglclock.1

EXTRA_DIST = options-0.dtd ${man_MANS}

