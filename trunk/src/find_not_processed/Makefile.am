bin_SCRIPTS = find_not_processed
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = find_not_processed.in
find_not_processed: find_not_processed.in Makefile
	m4 -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins -I$(srcdir)/../perllib hackery.m4 $< > $@
	chmod +x $@
