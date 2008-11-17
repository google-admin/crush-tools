bin_SCRIPTS = add_field
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab add_field.in test.conf \
							tests/test.in tests/test.first.expect tests/test.last.expect \
							tests/test.second.expect tests/test.third.expect \
							tests/test_00.sh tests/test_01.sh tests/test_02.sh

add_field: add_field.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins \
			  -I$(srcdir)/../perllib hackery.m4 $< crush_version.pl > $@
	chmod +x $@
man1_MANS = add_field.1
add_field.1 : args.tab
	../bin/genman.pl args.tab > $@

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf
