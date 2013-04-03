main.c usage.c fieldsplit_main.h : args.tab
	"$(CGENER)" -a ./args.tab

BUILT_SOURCES = main.c usage.c fieldsplit_main.h

bin_PROGRAMS = fieldsplit
fieldsplit_SOURCES = fieldsplit.c $(BUILT_SOURCES)

fieldsplit_LDADD = ../libcrush/libcrush.la

TESTS_ENVIRONMENT = $(top_srcdir)/src/bin/testharness.sh
TESTS = test.conf

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = args.tab test.conf \
             test/test_001.sh test/001-transform-key.txt test/002-data.txt \
             test/001/2008-01.expected test/001/2008-02.expected \
             test/test_002.sh test/002 \
             test/test_003.sh test/003/2008-01-29.expected \
             test/003/2008-02-01.expected \
             test/003/2008-02-09.expected \
             test/test_004.sh test/004/1.expected \
             test/004/2.expected \
             test/test_005.sh test/005/10.expected \
             test/005/11.expected test/005/_blank_value.expected \
             test/test_006.sh test/006/0.expected \
						 test/006/1.expected test/006/2.expected \
						 test/test_007.sh test/007/1.expected test/007/2.expected \
						 test/test_008.sh test/008/10.expected \
						 test/008/11.expected test/008/_blank_value.expected

man1_MANS = fieldsplit.1
fieldsplit.1 : args.tab
	../bin/genman.pl args.tab > $@
