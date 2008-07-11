bin_SCRIPTS = add_field
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = add_field.in

add_field: add_field.in Makefile
	m4 -D 'CRUSH_PERL_SHEBANG=#!$(PERL)' --prefix-builtins $< > $@
	chmod +x $@
