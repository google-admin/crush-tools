main.c usage.c deltaforce_main.h : args.tab
	$(CGENER) -a ./args.tab

BUILT_SOURCES = main.c usage.c deltaforce_main.h

bin_PROGRAMS = deltaforce
deltaforce_SOURCES = deltaforce.c deltaforce.h $(BUILT_SOURCES)

deltaforce_LDADD = ../libcrush/libcrush.la

TESTS = tests.sh
