bin_SCRIPTS = find_not_processed
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab find_not_processed.in test.sh
find_not_processed: find_not_processed.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins \
				-I$(srcdir)/../perllib hackery.m4 $< crush_version.pl > $@
	chmod +x $@

TESTS = test.sh
man1_MANS = find_not_processed.1
find_not_processed.1 : args.tab
	../bin/genman.pl args.tab > $@
