BUILT_SOURCES = main.c usage.c funiq_main.h


$(BUILT_SOURCES) : args.tab
	$(CGENER) -a ./args.tab


bin_PROGRAMS = funiq
funiq_SOURCES = funiq.c funiq.h $(BUILT_SOURCES)

TESTS = tests.sh
CLEANFILES = $(BUILT_SOURCES)


funiq_LDADD = ../libcrush/libcrush.la
