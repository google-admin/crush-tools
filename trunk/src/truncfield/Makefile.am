BUILT_SOURCES = main.c usage.c truncfield_main.h

$(BUILT_SOURCES) : args.tab
	"$(CGENER)" -a ./args.tab

bin_PROGRAMS = truncfield
truncfield_SOURCES = truncfield.c truncfield.h $(BUILT_SOURCES)
truncfield_LDADD = ../libcrush/libcrush.la

TESTS = tests.sh
CLEANFILES = $(BUILT_SOURCES)

