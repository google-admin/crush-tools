bin_SCRIPTS = csvformat
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab csvformat.in
csvformat: csvformat.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins -I$(srcdir)/../perllib hackery.m4 $< > $@
	chmod +x $@
