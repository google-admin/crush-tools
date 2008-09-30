BUILT_SOURCES = main.c usage.c funiq_main.h

$(BUILT_SOURCES) : args.tab
	"$(CGENER)" -a ./args.tab

bin_PROGRAMS = funiq
funiq_SOURCES = funiq.c $(BUILT_SOURCES)

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

funiq_LDADD = ../libcrush/libcrush.la
EXTRA_DIST = args.tab test.conf \
             test/test_01.sh \
             test/test_02.sh
