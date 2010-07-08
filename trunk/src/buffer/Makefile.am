main.c usage.c aggregate_main.h : args.tab $(CGENER)
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c buffer_main.h

bin_PROGRAMS = buffer
buffer_SOURCES = buffer.c $(BUILT_SOURCES)
buffer_LDADD = ../libcrush/libcrush.la
CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab
man1_MANS = buffer.1
buffer.1 : args.tab ../bin/genman.pl
	../bin/genman.pl args.tab > $@

