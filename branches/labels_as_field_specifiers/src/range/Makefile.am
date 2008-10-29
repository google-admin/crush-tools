

bin_PROGRAMS = range
range_SOURCES = range.c
range_LDADD = ../libcrush/libcrush.la
EXTRA_DIST = args.tab
man1_MANS = range.1
range.1 : args.tab
	../bin/genman.pl args.tab > $@
