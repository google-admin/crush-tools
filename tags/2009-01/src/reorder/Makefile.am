main.c usage.c reorder_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c reorder_main.h

bin_PROGRAMS = reorder
reorder_SOURCES = reorder.c reorder.h $(BUILT_SOURCES)
reorder_LDADD = ../libcrush/libcrush.la

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab test.conf tests/test.in0 tests/test.in1 tests/test.in2 \
						 tests/test_labeled.in \
             tests/test_00.sh tests/test_01.sh tests/test_01.0.expected \
             tests/test_02.sh tests/test_02.1.expected \
						 tests/test_03.sh tests/test_03.expected \
						 tests/test_04.sh tests/test_04.0.expected \
						 tests/test_04.1.expected tests/test_04.2.expected \
						 tests/test_04.3.expected tests/test_04.4.expected \
						 tests/test_04.5.expected

man1_MANS = reorder.1
reorder.1 : args.tab
	../bin/genman.pl args.tab > $@
