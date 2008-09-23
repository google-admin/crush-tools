bin_SCRIPTS = calcfield
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab calcfield.in
calcfield: calcfield.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins \
				-I$(srcdir)/../perllib hackery.m4 $< crush_version.pl > $@
	chmod +x $@

# TESTS = test.sh
