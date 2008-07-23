main.c usage.c reorder_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c reorder_main.h

bin_PROGRAMS = reorder
reorder_SOURCES = reorder.c reorder.h $(BUILT_SOURCES)
reorder_LDADD = ../libcrush/libcrush.la

TESTS = reorder.test.sh

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab
