BUILT_SOURCES = main.c usage.c subtotal_main.h

$(BUILT_SOURCES) : args.tab
	"$(CGENER)" -a ./args.tab

bin_PROGRAMS = subtotal
subtotal_SOURCES = subtotal.c $(BUILT_SOURCES)
subtotal_LDADD = ../libcrush/libcrush.la

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab test.conf test/test_01.sh test/test_02.sh \
						 test/test_03.sh test/test_04.sh

man1_MANS = subtotal.1
subtotal.1 : args.tab
	../bin/genman.pl args.tab > $@
