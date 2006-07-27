DOCDIR = ../../docs
DOXYFILE = hashtbl.doxygen

CC	:= gcc
LD	:= ld

INCDIR	:= ~/include/
LIBDIR	:= ~/lib/

OBJECTS := $(wildcard *.o) $(wildcard ../linklist/*.o)

CFLAGS	+= -I. -g -I../include -I$(INCDIR) -L../lib -L$(LIBDIR) -O2 -Wall -fPIC
# -llinklist
LDFLAGS := -dy -G -z text 
AR      := ar
ARFLAGS := rcs

.PHONY : docs

default : libhashtbl.a

all : libhashtbl.a

test :

install : libhashtbl.a hashtbl.h hashfuncs.h
	cp libhashtbl.a $(LIBDIR)
	cp hashtbl.h $(INCDIR)
	cp hashfuncs.h $(INCDIR)

so : libhashtbl.so.1.0 

libhashtbl.so.1.0 : $(OBJECTS)
	$(LD) $(LDFLAGS) -o libhashtbl.so.1.0 $(OBJECTS)

hashtbl.o : hashtbl.c hashtbl.h
	${CC} ${CFLAGS} -c -o hashtbl.o hashtbl.c

hashfuncs.o : hashfuncs.c hashfuncs.h
	${CC} ${CFLAGS} -c -o hashfuncs.o hashfuncs.c

libhashtbl.a : hashtbl.o hashfuncs.o
	$(AR) $(ARFLAGS) libhashtbl.a $(OBJECTS)
	cp hashtbl.h ../include/

clean :
	rm *.o

cleanall :
	rm *.o libhashtbl.a

docs :
	@if [ ! -d $(DOCDIR) ]; then mkdir $(DOCDIR); fi; \
	doxygen $(DOXYFILE)
