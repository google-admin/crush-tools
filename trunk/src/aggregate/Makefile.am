main.c usage.c aggregate_main.h : args.tab
	$(CGENER) -a ./args.tab

BUILT_SOURCES = main.c usage.c aggregate_main.h

bin_PROGRAMS = aggregate
aggregate_SOURCES = aggregate.c aggregate.h $(BUILT_SOURCES)

TESTS = test.sh
# aggregate_LDADD = -lffutils -ljdatastructs
