BUILT_SOURCES = main.c usage.c convdate_main.h

$(BUILT_SOURCES) : args.tab
	"$(CGENER)" -a ./args.tab

bin_PROGRAMS = convdate
convdate_SOURCES = convdate.c convdate.h $(BUILT_SOURCES)
convdate_LDADD = ../libcrush/libcrush.la

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab test.conf \
             test/test_01.sh \
             test/test_02.sh
man1_MANS = convdate.1
convdate.1 : args.tab
	../bin/genman.pl args.tab > $@
