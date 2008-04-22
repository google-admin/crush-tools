BUILT_SOURCES = main.c usage.c foreach_parallel_main.h


$(BUILT_SOURCES) : args.tab
	$(CGENER) -a ./args.tab


bin_PROGRAMS = foreach_parallel
foreach_parallel_SOURCES = foreach_parallel.c foreach_parallel.h $(BUILT_SOURCES)

TESTS = tests.sh
CLEANFILES = $(BUILT_SOURCES)


