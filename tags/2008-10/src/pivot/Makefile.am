main.c usage.c pivot_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c pivot_main.h

bin_PROGRAMS = pivot
pivot_SOURCES = pivot.c $(BUILT_SOURCES)
pivot_LDADD = ../libcrush/libcrush.la

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab test.conf \
             tests/test_00.sh tests/test_00.in tests/test_00.expected \
             tests/test_01.sh tests/test_01.in tests/test_01.expected \
             tests/test_02.sh tests/test_02.in tests/test_02.expected

