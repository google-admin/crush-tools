main.c usage.c grepfield_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c grepfield_main.h

bin_PROGRAMS = grepfield
grepfield_SOURCES = grepfield.c grepfield.h $(BUILT_SOURCES)

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

grepfield_LDADD = ../libcrush/libcrush.la
CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab test.conf test/test.in \
             test/test_00.sh test/test_00.expected \
             test/test_01.sh test/test_01.expected \
             test/test_02.sh test/test_02.expected \
             test/test_03.sh test/test_03.expected

