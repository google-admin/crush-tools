BUILT_SOURCES = main.c usage.c convdate_main.h

$(BUILT_SOURCES) : args.tab
	"$(CGENER)" -a ./args.tab

bin_PROGRAMS = convdate
convdate_SOURCES = convdate.c convdate.h $(BUILT_SOURCES)

TESTS = tests.sh

convdate_LDADD = ../libcrush/libcrush.la

CLEANFILES = $(BUILT_SOURCES)

