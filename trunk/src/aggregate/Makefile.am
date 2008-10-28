main.c usage.c aggregate_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c aggregate_main.h

bin_PROGRAMS = aggregate
aggregate_SOURCES = aggregate.c aggregate.h $(BUILT_SOURCES)

aggregate_LDADD = ../libcrush/libcrush.la

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab test.conf test/test.in \
             test/test_00.sh test/test_00.expected \
             test/test_01.sh test/test_01.expected \
						 test/test_02.sh test/test_02.expected
man1_MANS = aggregate.1
aggregate.1 : args.tab
	../bin/genman.pl args.tab > $@
