PATH_PREFIX = ~
HTML_DOC_PATH = $(PATH_PREFIX)/public_html/lib/c/mempool/

CC	:= gcc

CFLAGS	:= -I. -O2 -Wall
LDFLAGS :=
AR      := ar -r

INCDIR	:= $(PATH_PREFIX)/include/
LIBDIR	:= $(PATH_PREFIX)/lib/

default : libmempool.a

test : libmempool.a mempool_test.c
	$(CC) -L. -I. -o mempool_test mempool_test.c -lmempool && \
	./mempool_test && \
	rm ./mempool_test

install : libmempool.a mempool.h
	cp libmempool.a $(LIBDIR)
	cp mempool.h $(INCDIR)

mempool.o : mempool.c mempool.h
	$(CC) $(CFLAGS) -c -o mempool.o mempool.c

libmempool.a : mempool.o
	${AR} libmempool.a mempool.o

clean :
	rm *.o

cleanall :
	rm *.o libmempool.a

docs : mempool.c mempool.h mempool.doxygen
	doxygen mempool.doxygen

docs-install : docs
	cp docs/html/* $(HTML_DOC_PATH)
