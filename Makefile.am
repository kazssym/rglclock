## Process this file with automake to produce a Makefile.in.

AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	$(GTK_CFLAGS)

bin_PROGRAMS = rglclock
noinst_LIBRARIES = libgdkgl.a libclock.a

rglclock_SOURCES = rglclock.cpp profile.cpp getopt.c getopt1.c
rglclock_LDADD = libclock.a libgdkgl.a $(GL_LIBS) $(GTK_LIBS)

libgdkgl_a_SOURCES = gdkgl.c

libclock_a_SOURCES = simple.c module.cc glclock.cc \
	options.cc clockoptions.cc about.cc

noinst_HEADERS = options.h profile.h getopt.h glclock.h module.h simple.h \
	autowidget.h gdkgl.h

man_MANS = rglclock.1

EXTRA_DIST = README.ja $(man_MANS)

SUBDIRS = intl po

dist-hook:
	cd $(distdir); \
find . ! -name SHA1SUMS -type f -print | sed 's,^\./,,' | xargs sha1sum > SHA1SUMS \
|| rm -f SHA1SUMS
	if test -f $(srcdir)/sign-distdir.sh; then \
  $(SHELL) $(srcdir)/sign-distdir.sh $(distdir); \
fi
