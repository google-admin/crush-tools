main.c usage.c aggregate_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c aggregate_main.h

bin_PROGRAMS = aggregate
aggregate_SOURCES = aggregate.c aggregate.h $(BUILT_SOURCES)

aggregate_LDADD = ../libcrush/libcrush.la

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab test.conf test/test.in test/test.in2 \
	test/test_00.sh test/test_00.expected \
	test/test_01.sh test/test_01.expected \
	test/test_02.sh test/test_02.expected \
	test/test_03.sh test/test_03.expected \
	test/test_04.sh test/test_04.expected \
	test/test_05.sh test/test_05.0.C.expected \
	test/test_05.0.en_US.utf8.expected test/test_05.0.es_AR.utf8.expected \
	test/test_05.1.C.expected test/test_05.1.en_US.utf8.expected \
	test/test_05.1.es_AR.utf8.expected test/test_05.2.C.expected \
	test/test_05.2.en_US.utf8.expected test/test_05.2.es_AR.utf8.expected \
	test/test_06.sh test/test_06.0.expected \
	test/test_06.1.expected test/test_06.2.expected

man1_MANS = aggregate.1
aggregate.1 : args.tab
	../bin/genman.pl args.tab > $@
