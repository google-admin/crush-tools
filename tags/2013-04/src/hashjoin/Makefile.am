main.c usage.c hashjoin_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c hashjoin_main.h

bin_PROGRAMS = hashjoin
hashjoin_SOURCES = hashjoin.c $(BUILT_SOURCES)

hashjoin_LDADD = ../libcrush/libcrush.la

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab test.conf \
             test/dimension_header.log \
             test/dimension_no_header.log \
             test/input_header.log \
             test/input_no_header.log \
             test/test_00.expected \
             test/test_00.sh \
             test/test_01.expected \
             test/test_01.sh \
             test/test_02.expected \
             test/test_02.sh \
             test/test_03.expected \
             test/test_03.sh

man1_MANS = hashjoin.1
hashjoin.1 : args.tab
	../bin/genman.pl args.tab > $@
