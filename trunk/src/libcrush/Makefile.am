
lib_LTLIBRARIES = libcrush.la
libcrush_la_SOURCES = GeneralHashFunctions.c bstree.c ffutils.c hashfuncs.c \
                      hashtbl.c hashtbl2.c linklist.c mempool.c qsort_helper.c \
											queue.c dbfr.c

libcrush_la_LDFLAGS = -version-info 1:0:0

check_PROGRAMS = test/dbfr_test test/ffutils_test \
							   test/mempool_test test/qsort_helper_test

TESTS = $(check_PROGRAMS)
# test/datecmp_test 
# test_datecmp_test_LDADD = libcrush.la
test_dbfr_test_LDADD = libcrush.la
test_ffutils_test_LDADD = libcrush.la
test_mempool_test_LDADD = libcrush.la
test_qsort_helper_test_LDADD = libcrush.la
