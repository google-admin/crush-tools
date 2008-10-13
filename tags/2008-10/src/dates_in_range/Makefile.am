bin_SCRIPTS = dates_in_range
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab dates_in_range.in
dates_in_range: dates_in_range.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins \
				-I$(srcdir)/../perllib hackery.m4 $< crush_version.pl > $@
	chmod +x $@
