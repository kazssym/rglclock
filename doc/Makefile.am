## Process this file with automake to produce a Makefile.in.

DOCBOOK2TEXI = docbook2texi

info_TEXINFOS = rglclock.texi

EXTRA_DIST = rglclock.xml

${srcdir}/rglclock.texi: rglclock.xml
	${DOCBOOK2TEXI} ${srcdir}/rglclock.xml > $@
	if grep '^@setfilename' $@; then :; else \
	    head -n 1 $@ > $@.tmp && \
	    echo '@setfilename rglclock.info' >> $@.tmp && \
	    echo '@settitle rglclock' >> $@.tmp && \
	    tail -n +2 $@ >> $@.tmp && \
	    mv $@.tmp $@; \
	fi
