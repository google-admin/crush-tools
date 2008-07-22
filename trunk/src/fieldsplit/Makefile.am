bin_SCRIPTS = fieldsplit
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab fieldsplit.in
fieldsplit: fieldsplit.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins -I$(srcdir)/../perllib hackery.m4 $< > $@
	chmod +x $@
