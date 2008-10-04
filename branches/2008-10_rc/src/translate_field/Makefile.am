bin_SCRIPTS = translate_field
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab translate_field.in
translate_field: translate_field.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins \
				-I$(srcdir)/../perllib hackery.m4 $< crush_version.pl > $@
	chmod +x $@
