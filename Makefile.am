## Process this file with automake to produce a Makefile.in.

EXTRA_DIST = README.ja MEMO.ja VERSION-ID

SUBDIRS = intl po include lib bin

update-po:
	cd po && ${MAKE} update-po

dist-snapshot distcheck-snapshot:
	${MAKE} VERSION=${VERSION}.`date +%Y%m%d` `echo $@ | sed -e 's/-snapshot//'`

