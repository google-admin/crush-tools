CC	:= gcc

CFLAGS	:= -I. -O2 -Wall
LDFLAGS :=
AR      := ar -r

INCDIR	:= /home/jhinds/include/
LIBDIR	:= /home/jhinds/lib/

default : libffutils.a

test :

install : libffutils.a ffutils.h
	cp libffutils.a $(LIBDIR)
	cp ffutils.h $(INCDIR)

ffutils.o : ffutils.c ffutils.h
	${CC} ${CFLAGS} -c -o ffutils.o ffutils.c

libffutils.a : ffutils.o
	${AR} libffutils.a ffutils.o

clean :
	rm *.o

cleanall :
	rm *.o libffutils.a

docs : ffutils.c ffutils.h ffutils.doxygen
	doxygen ffutils.doxygen

docs-install : docs
	cp docs/html/* ~/public_html/lib/c/ffutils/
