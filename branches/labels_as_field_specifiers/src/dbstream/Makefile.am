bin_SCRIPTS = dbstream
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab dbstream.in test.conf tests/setup.pl \
             tests/test_00.sh tests/test_01.sh tests/test_02.sh \
             tests/test_03.sh

dbstream: dbstream.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins \
				-I$(srcdir)/../perllib hackery.m4 $< crush_version.pl > $@
	chmod +x $@

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf
