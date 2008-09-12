main.c usage.c mergekeys_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c mergekeys_main.h

bin_PROGRAMS = mergekeys
mergekeys_SOURCES = mergekeys.c mergekeys.h $(BUILT_SOURCES)
mergekeys_LDADD = ../libcrush/libcrush.la

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab
