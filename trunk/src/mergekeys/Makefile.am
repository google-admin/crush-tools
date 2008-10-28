main.c usage.c mergekeys_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c mergekeys_main.h

bin_PROGRAMS = mergekeys
mergekeys_SOURCES = mergekeys.c mergekeys.h $(BUILT_SOURCES)
mergekeys_LDADD = ../libcrush/libcrush.la

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab test.conf \
             tests/test_00.sh tests/test_00.a tests/test_00.b \
						 tests/test_00.expected \
             tests/test_01.sh tests/test_01.a tests/test_01.b \
						 tests/test_01.expected \
             tests/test_02..expected tests/test_02.-i.expected \
						 tests/test_02.-l.expected tests/test_02.-r.expected \
             tests/test_02.a tests/test_02.b tests/test_02.sh \
             tests/test_03..expected tests/test_03.-i.expected \
						 tests/test_03.-l.expected tests/test_03.-r.expected \
             tests/test_03.a tests/test_03.b tests/test_03.sh \
						 tests/test_04.sh tests/test_04.a tests/test_04.b \
						 tests/test_04..expected tests/test_04.-i.expected \
             tests/test_04.-l.expected tests/test_04.-r.expected \
             tests/test_05.sh tests/test_05.a tests/test_05.b \
						 tests/test_05.-r.expected tests/test_05..expected \
             tests/test_05.-l.expected tests/test_05.-i.expected \
						 tests/test_06.sh tests/test_06.a tests/test_06.b \
             tests/test_06.-r.expected tests/test_06.-i.expected \
						 tests/test_06.-l.expected  tests/test_06..expected \
             tests/test_07.sh tests/test_07.a tests/test_07.b \
             tests/test_07..expected tests/test_07.-r.expected \
						 tests/test_07.-i.expected tests/test_07.-l.expected \
						 tests/test_08.a tests/test_08.b tests/test_08.sh \
             tests/test_08..expected tests/test_08.-i.expected \
						 tests/test_08.-l.expected tests/test_08.-r.expected \
						 tests/test_09.sh tests/test_09.a tests/test_09.b \
             tests/test_09.expected 
man1_MANS = mergekeys.1
mergekeys.1 : args.tab
	../bin/genman.pl args.tab > $@
