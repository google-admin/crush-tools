main.c usage.c deltaforce_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c deltaforce_main.h

bin_PROGRAMS = deltaforce
deltaforce_SOURCES = deltaforce.c deltaforce.h $(BUILT_SOURCES)

deltaforce_LDADD = ../libcrush/libcrush.la

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab test.conf \
             tests/test_00.sh tests/test_00-full.txt \
						 tests/test_00-delta.txt tests/test_00.expected \
             tests/test_01.sh tests/test_01-full.txt \
						 tests/test_01-delta.txt tests/test_01.expected \
             tests/test_02.sh tests/test_02-full.txt \
						 tests/test_02-delta.txt tests/test_02.expected \
             tests/test_03.sh tests/test_03-full.txt \
						 tests/test_03-delta.txt tests/test_03.expected \
             tests/test_04.sh tests/test_04-full.txt \
						 tests/test_04-delta.txt tests/test_04.expected
man1_MANS = deltaforce.1
deltaforce.1 : args.tab
	../bin/genman.pl args.tab > $@
