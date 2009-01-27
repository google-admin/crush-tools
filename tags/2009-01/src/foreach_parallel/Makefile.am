main.c usage.c foreach_parallel_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c foreach_parallel_main.h

bin_PROGRAMS = foreach_parallel
foreach_parallel_SOURCES = foreach_parallel.c $(BUILT_SOURCES)

# TESTS = tests.sh

CLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST = args.tab
man1_MANS = foreach_parallel.1
foreach_parallel.1 : args.tab
	../bin/genman.pl args.tab > $@
