main.c usage.c reorder_main.h : args.tab
	$(CGENER) -a ./args.tab

BUILT_SOURCES = main.c usage.c reorder_main.h

bin_PROGRAMS = reorder
reorder_SOURCES = reorder.c reorder.h $(BUILT_SOURCES)

TESTS = reorder.test.sh

