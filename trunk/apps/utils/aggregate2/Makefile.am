main.c usage.c aggregate2_main.h : args.tab
	$(CGENER) -a ./args.tab

BUILT_SOURCES = main.c usage.c aggregate2_main.h

bin_PROGRAMS = aggregate2
aggregate2_SOURCES = aggregate2.c $(BUILT_SOURCES)

TESTS = test.sh
# aggregate_LDADD = -lffutils -ljdatastructs
