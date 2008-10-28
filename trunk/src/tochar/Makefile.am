
bin_PROGRAMS = tochar
tochar_SOURCES = tochar.c

EXTRA_DIST = args.tab
man1_MANS = tochar.1
tochar.1 : args.tab
	../bin/genman.pl args.tab > $@
