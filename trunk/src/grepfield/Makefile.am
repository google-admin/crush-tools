main.c usage.c grepfield_main.h : args.tab
	$(CGENER) -a ./args.tab

BUILT_SOURCES = main.c usage.c grepfield_main.h

bin_PROGRAMS = grepfield
grepfield_SOURCES = grepfield.c grepfield.h $(BUILT_SOURCES)

TESTS = test.sh

