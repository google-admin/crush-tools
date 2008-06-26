main.c usage.c pivot_main.h : args.tab
	$(CGENER) -a ./args.tab

BUILT_SOURCES = main.c usage.c pivot_main.h

bin_PROGRAMS = pivot
pivot_SOURCES = pivot.c $(BUILT_SOURCES)
pivot_LDADD = ../libcrush/libcrush.la

TESTS = tests.sh

