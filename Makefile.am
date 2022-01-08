## Process this file with automake to produce a Makefile.in.

AUTOMAKE_OPTIONS = 1.9
ACLOCAL_AMFLAGS = -Im4

EXTRA_DIST = README.ja MEMO.ja

SUBDIRS = $(subdirs) libgnu lib modules programs testsuite doc po

dist-hook:
	cd $(distdir); \
find . ! -name SHA1SUMS -type f -print | sed 's,^\./,,' | xargs sha1sum > SHA1SUMS \
|| rm -f SHA1SUMS
	if test -f $(srcdir)/sign-distdir.sh; then \
  $(SHELL) $(srcdir)/sign-distdir.sh $(distdir); \
fi
