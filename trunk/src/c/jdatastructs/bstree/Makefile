DOCDIR = ../../docs
DOXYFILE = bstree.doxygen

CC	:= gcc
LD	:= ld

CFLAGS	+= -g -I. -I../include -O2 -Wall -fPIC
LDFLAGS := ld -dy -G -z text
AR      := ar -r

INCDIR	:= ~/include/
LIBDIR	:= ~/lib/

.PHONY : docs

default : libbstree.a

all : libbstree.a

test :

install : libbstree.a bstree.h
	cp libbstree.a $(LIBDIR)
	cp bstree.h $(INCDIR)

so : libbstree.so.1.0 

libbstree.so.1.0 : bstree.o
	$(LD) $(LDFLAGS) -o libbstree.so.1.0

bstree.o : bstree.c bstree.h
	${CC} ${CFLAGS} -c -o bstree.o bstree.c

libbstree.a : bstree.o
	${AR} libbstree.a bstree.o
	cp bstree.h ../include/

clean :
	rm *.o

cleanall :
	rm *.o libbstree.a

docs :
	@if [ ! -d $(DOCDIR) ]; then mkdir $(DOCDIR); fi; \
	doxygen $(DOXYFILE)
