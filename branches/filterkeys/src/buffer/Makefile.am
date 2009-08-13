
bin_PROGRAMS = buffer
buffer_SOURCES = buffer.c
buffer_LDADD = ../libcrush/libcrush.la

EXTRA_DIST = args.tab
man1_MANS = buffer.1
buffer.1 : args.tab
	../bin/genman.pl args.tab > $@
