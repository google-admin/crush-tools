CC	:= gcc

CFLAGS	:= -I. -O2 -Wall -D_LARGEFILE64_SOURCE
LDFLAGS :=
AR      := ar -r

INCDIR	:= $(HOME)/include/
LIBDIR	:= $(HOME)/lib/

default : libffutils.a

test : libffutils.a
	$(CC) -g -L. -o ffutils_test ffutils_test.c -lffutils
	./ffutils_test
	rm ffutils_test

install : libffutils.a ffutils.h data_transfer.h 
	cp libffutils.a $(LIBDIR)
	cp ffutils.h data_transfer.h $(INCDIR)

ffutils.o : ffutils.c ffutils.h
	${CC} ${CFLAGS} -c -o ffutils.o ffutils.c

libffutils.a : ffutils.o
	${AR} libffutils.a ffutils.o

clean :
	rm *.o

cleanall :
	rm *.o libffutils.a

docs : ffutils.c ffutils.h data_transfer.h ffutils.doxygen
	doxygen ffutils.doxygen

docs-install : docs
	cp docs/html/* ~/public_html/lib/c/ffutils/
