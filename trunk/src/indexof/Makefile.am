BUILT_SOURCES = main.c usage.c indexof_main.h


$(BUILT_SOURCES) : args.tab
	$(CGENER) -a ./args.tab


bin_PROGRAMS = indexof
indexof_SOURCES = indexof.c indexof.h $(BUILT_SOURCES)

TESTS = tests.sh
CLEANFILES = $(BUILT_SOURCES)

indexof_LDADD = ../libcrush/libcrush.la
