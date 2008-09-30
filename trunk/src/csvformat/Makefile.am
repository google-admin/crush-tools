bin_SCRIPTS = csvformat
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab csvformat.in test.conf \
             tests/test_00.sh tests/test_00.expected \
             tests/test_01.sh tests/test_02.sh \
             tests/test_03.sh tests/test_03.in tests/test_03.expected

csvformat: csvformat.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins \
				-I$(srcdir)/../perllib hackery.m4 $< crush_version.pl > $@
	chmod +x $@

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

