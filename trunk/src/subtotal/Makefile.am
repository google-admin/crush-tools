BUILT_SOURCES = main.c usage.c subtotal_main.h

$(BUILT_SOURCES) : args.tab
	$(CGENER) -a ./args.tab


bin_PROGRAMS = subtotal
subtotal_SOURCES = subtotal.c subtotal.h $(BUILT_SOURCES)
subtotal_LDADD = ../libcrush/libcrush.la

TESTS = tests.sh
CLEANFILES = $(BUILT_SOURCES)

