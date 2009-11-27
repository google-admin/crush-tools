main.c usage.c treesort_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c treesort_main.h

bin_PROGRAMS = treesort
treesort_SOURCES = treesort.c treesort.h $(BUILT_SOURCES)

treesort_LDADD = ../libcrush/libcrush.la

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab test.conf test/test.in test/test2.in \
						test/test_00.0.expected test/test_00.1.expected \
						test/test_00.2.expected test/test_00.3.expected \
						test/test_00.sh \
						test/test_01.0.expected test/test_01.1.expected \
						test/test_01.2.expected test/test_01.3.expected \
						test/test_01.4.expected \
						test/test_01.sh \
						test/test_02.0.expected test/test_02.1.expected \
						test/test_02.2.expected test/test_02.3.expected \
						test/test_02.4.expected test/test_02.5.expected \
						test/test_02.6.expected test/test_02.7.expected \
						test/test_02.8.expected test/test_02.9.expected \
						test/test_02.sh \
						test/test_03.0.expected test/test_03.1.expected \
						test/test_03.2.expected test/test_03.3.expected \
						test/test_03.4.expected test/test_03.5.expected \
						test/test_03.6.expected test/test_03.7.expected \
						test/test_03.sh

man1_MANS = treesort.1
treesort.1 : args.tab
	../bin/genman.pl args.tab > $@
