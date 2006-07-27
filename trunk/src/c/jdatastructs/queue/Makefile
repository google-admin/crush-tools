DOCDIR = ../../docs
DOXYFILE = queue.doxygen

CC	:= gcc
LD	:= ld

OBJECTS := $(wildcard *.o) $(wildcard ../linklist/*.o)
CFLAGS	+= -I. -g -I../include -L../lib -O2 -Wall -fPIC
# -llinklist
LDFLAGS := -dy -G -z text 
AR      := ar
ARFLAGS := rcs
INCDIR	:= ~/include/
LIBDIR	:= ~/lib/

.PHONY : docs

default : libqueue.a

all : libqueue.a

test :

install : libqueue.a queue.h
	cp libqueue.a $(LIBDIR)
	cp queue.h $(INCDIR)

so : libqueue.so.1.0 

libqueue.so.1.0 : $(OBJECTS)
	$(LD) $(LDFLAGS) -o libqueue.so.1.0 $(OBJECTS)

queue.o : queue.c queue.h
	${CC} ${CFLAGS} -c -o queue.o queue.c

libqueue.a : queue.o
	$(AR) $(ARFLAGS) libqueue.a $(OBJECTS)
	cp queue.h ../include

clean :
	rm *.o

cleanall :
	rm *.o libqueue.a

docs :
	@if [ ! -d $(DOCDIR) ]; then mkdir $(DOCDIR); fi; \
	doxygen $(DOXYFILE)
