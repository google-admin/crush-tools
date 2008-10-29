bin_SCRIPTS = fieldsplit
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = args.tab fieldsplit.in test.pl \
             test/001-transform-key.txt test/002-data.txt \
             test/001/2008-01.expected test/001/2008-02.expected \
             test/002 \
             test/003/2008-01-29.expected \
             test/003/2008-02-01.expected \
             test/003/2008-02-09.expected \
             test/004/1.expected \
             test/004/2.expected \
             test/005/10.expected \
             test/005/11.expected \
             test/005/_blank_value.expected


fieldsplit: fieldsplit.in Makefile
	$(M4) -D"CRUSH_PERL_PATH=$(PERL)" --prefix-builtins \
				-I$(srcdir)/../perllib hackery.m4 $< crush_version.pl > $@
	chmod +x $@

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf
man1_MANS = fieldsplit.1
fieldsplit.1 : args.tab
	../bin/genman.pl args.tab > $@
