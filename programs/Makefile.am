## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I ../libgnu -I $(top_srcdir)/libgnu -I $(top_srcdir)/include \
	-DPKGDATADIR=\"$(pkgdatadir)\" -DLOCALEDIR=\"$(localedir)\" \
	-DOPTIONS_DTD=\"$(pkgdatadir)/options-0.dtd\" \
	$(GTK_CFLAGS) $(LTDL_CPPFLAGS) $(LIBXML2_CFLAGS) $(LIBPNG_CFLAGS)

bin_PROGRAMS = rglclock

rglclock_LDFLAGS = -dlopen ../modules/libplain.la

rglclock_SOURCES = rglclock.cpp about.cpp profile.cpp clock.cpp \
	options.cpp modaldialog.cpp glgtk.cpp
rglclock_LDADD = ../lib/librglclock.la ../lib/libsimple.a ../libgnu/libgnu.la \
	$(GTK_LIBS) $(LTDL_LIBS) $(LIBPNG_LIBS) $(LIBXML2_LIBS)

noinst_HEADERS = about.h profile.h clock.h glgtk.h

man_MANS = rglclock.1

EXTRA_DIST = $(man_MANS)

