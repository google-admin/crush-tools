bin_SCRIPTS = dbstream
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = dbstream.in
dbstream: dbstream.in Makefile
	m4 -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins -I$(srcdir)/../perllib hackery.m4 $< > $@
	chmod +x $@
