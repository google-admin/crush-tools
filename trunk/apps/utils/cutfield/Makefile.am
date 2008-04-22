BUILT_SOURCES = main.c usage.c cutfield_main.h


$(BUILT_SOURCES) : args.tab
	$(CGENER) -a ./args.tab


bin_PROGRAMS = cutfield
cutfield_SOURCES = cutfield.c cutfield.h $(BUILT_SOURCES)

TESTS = tests.sh


