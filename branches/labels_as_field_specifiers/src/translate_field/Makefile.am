bin_SCRIPTS = translate_field
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab translate_field.in \
						 test.conf test/test.in \
						 test/test_00.sh test/test_00.expected \
						 test/test_01.sh test/test_01.expected \
						 test/test_02.sh test/test_02.expected \
						 test/test_03.sh test/test_03.expected

translate_field: translate_field.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins \
				-I$(srcdir)/../perllib hackery.m4 $< crush_version.pl > $@
	chmod +x $@
man1_MANS = translate_field.1
translate_field.1 : args.tab
	../bin/genman.pl args.tab > $@

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf
