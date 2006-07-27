DOCDIR = ../../docs
DOXYFILE = linklist.doxygen

CC	:= gcc
LD	:= ld

CFLAGS	:= -I. -I../include -O2 -Wall -fPIC
LDFLAGS := ld -dy -G -z text
AR      := ar -r

INCDIR	:= ~/include/
LIBDIR	:= ~/lib/

.PHONY : docs

default : liblinklist.a

all : liblinklist.a

test :

install : liblinklist.a linklist.h
	cp liblinklist.a $(LIBDIR)
	cp linklist.h $(INCDIR)

so : liblinklist.so.1.0 

liblinklist.so.1.0 : linklist.o
	$(LD) $(LDFLAGS) -o liblinklist.so.1.0

linklist.o : linklist.c linklist.h
	${CC} ${CFLAGS} -c -o linklist.o linklist.c

liblinklist.a : linklist.o
	${AR} liblinklist.a linklist.o
	cp linklist.h ../include/

clean :
	rm *.o

cleanall :
	rm *.o liblinklist.a

docs :
	@if [ ! -d $(DOCDIR) ]; then mkdir $(DOCDIR); fi; \
	doxygen $(DOXYFILE)
