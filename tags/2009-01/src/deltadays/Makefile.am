bin_SCRIPTS = deltadays
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab deltadays.in
deltadays: deltadays.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins \
				-I$(srcdir)/../perllib hackery.m4 $< crush_version.pl > $@
	chmod +x $@
man1_MANS = deltadays.1
deltadays.1 : args.tab
	../bin/genman.pl args.tab > $@
