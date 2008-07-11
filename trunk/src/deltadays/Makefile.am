bin_SCRIPTS = deltadays
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = deltadays.in
deltadays: deltadays.in Makefile
	m4 -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins -I$(srcdir)/../perllib hackery.m4 $< > $@
	chmod +x $@
