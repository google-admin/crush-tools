BUILT_SOURCES = main.c usage.c indexof_main.h

$(BUILT_SOURCES) : args.tab
	"$(CGENER)" -a ./args.tab

bin_PROGRAMS = indexof
indexof_SOURCES = indexof.c indexof.h $(BUILT_SOURCES)

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

indexof_LDADD = ../libcrush/libcrush.la
EXTRA_DIST = args.tab test.conf \
             test/test_01.sh \
             test/test_02.sh \
             test/test_03.sh
