main.c usage.c aggregate2_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c aggregate2_main.h

bin_PROGRAMS = aggregate2
aggregate2_SOURCES = aggregate2.c $(BUILT_SOURCES)

aggregate2_LDADD = ../libcrush/libcrush.la

TESTS = test.sh
CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab
