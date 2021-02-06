## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/include \
-I$(top_srcdir)/libgnu -I$(top_builddir)/libgnu \
-DPKGDATADIR=\"$(pkgdatadir)\" -DLOCALEDIR=\"$(localedir)\" \
-DOPTIONS_DTD=\"$(pkgdatadir)/options-0.dtd\" \
$(LIBPNG_CFLAGS) $(LIBXML2_CFLAGS) $(GTK_CFLAGS)

bin_PROGRAMS = rglclock

noinst_HEADERS = about.h profile.h glgtk.h

rglclock_LDFLAGS = -dlopen ../modules/libplain.la

rglclock_SOURCES = rglclock.cpp about.cpp profile.cpp \
	options.cpp modaldialog.cpp glgtk.cpp
rglclock_LDADD = ../lib/librglclock.la ../lib/libsimple.a ../libgnu/libgnu.la \
	$(LIBPNG_LIBS) $(LIBXML2_LIBS) $(GTK_LIBS) $(LTDL_LIBS)

man_MANS = rglclock.1

EXTRA_DIST = $(man_MANS)

