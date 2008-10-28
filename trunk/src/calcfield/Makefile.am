bin_SCRIPTS = calcfield
CLEANFILES = $(bin_SCRIPTS)

calcfield: calcfield.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins \
				-I$(srcdir)/../perllib hackery.m4 $< crush_version.pl > $@
	chmod +x $@

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

EXTRA_DIST = args.tab calcfield.in test.conf \
             test/imps_clks.log test/test.txt \
             test/test_00.sh test/test_00.expected \
             test/test_01.sh test/test_01.expected \
             test/test_02.sh test/test_02.expected \
             test/test_03.sh test/test_03.expected
man1_MANS = calcfield.1
calcfield.1 : args.tab
	../bin/genman.pl args.tab > $@
