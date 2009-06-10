
lib_LTLIBRARIES = libcrush.la
libcrush_la_SOURCES = GeneralHashFunctions.c bstree.c ffutils.c hashfuncs.c \
                      hashtbl.c hashtbl2.c linklist.c mempool.c qsort_helper.c \
                      queue.c dbfr.c reutils.c general.c

libcrush_includedir = $(includedir)/crush
libcrush_include_HEADERS = crush/bstree.h \
								           crush/config.h \
								           crush/crush_version.h \
								           crush/dbfr.h \
								           crush/ffutils.h \
								           crush/general.h \
								           crush/hashfuncs.h \
								           crush/hashtbl.h \
								           crush/hashtbl2.h \
								           crush/ht2_GeneralHashFunctions.h \
								           crush/linklist.h \
								           crush/mempool.h \
								           crush/qsort_helper.h \
								           crush/queue.h \
								           crush/reutils.h

libcrush_la_LDFLAGS = -version-info 1:0:0

check_PROGRAMS = test/dbfr_test test/ffutils_test \
							   test/mempool_test test/qsort_helper_test test/reutils_test

TESTS = $(check_PROGRAMS)
# test/datecmp_test
# test_datecmp_test_LDADD = libcrush.la
test_dbfr_test_LDADD = libcrush.la
test_ffutils_test_LDADD = libcrush.la
test_mempool_test_LDADD = libcrush.la
test_qsort_helper_test_LDADD = libcrush.la
test_reutils_test_LDADD = libcrush.la

EXTRA_DIST = $(check_PROGRAMS)

SUBDIRS = crush
