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
             tests/test_00.sh tests/test_01.sh tests/test_01.0.expected \
             tests/test_02.sh tests/test_02.1.expected

