BUILT_SOURCES = main.c usage.c cutfield_main.h

$(BUILT_SOURCES) : args.tab
	"$(CGENER)" -a ./args.tab

bin_PROGRAMS = cutfield
cutfield_SOURCES = cutfield.c $(BUILT_SOURCES)

TESTS = tests.sh

cutfield_LDADD = ../libcrush/libcrush.la
CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab
