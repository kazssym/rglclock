## Process this file with automake to produce a Makefile.in.

AUTOMAKE_OPTIONS = dejagnu

EXTRA_DIST = config/unix.exp

dist-hook:
	for tool in $(DEJATOOL); do \
  for dir in `cd $(srcdir) && echo $$tool.*`; do \
    if test -d $(srcdir)/$$dir; then \
      mkdir $(distdir)/$$dir; \
      cp -p $(srcdir)/$$dir/*.exp $(distdir)/$$dir 2> /dev/null; \
    fi; \
  done; \
done
