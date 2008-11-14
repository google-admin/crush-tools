bin_SCRIPTS = translate_field
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab translate_field.in \
						 test.conf tests/test.in \
						 tests/test_00.sh tests/test_00.expected \
						 tests/test_01.sh tests/test_01.expected \
						 tests/test_02.sh tests/test_02.expected \
						 tests/test_03.sh tests/test_03.expected

translate_field: translate_field.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins \
				-I$(srcdir)/../perllib hackery.m4 $< crush_version.pl > $@
	chmod +x $@
man1_MANS = translate_field.1
translate_field.1 : args.tab
	../bin/genman.pl args.tab > $@

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf
