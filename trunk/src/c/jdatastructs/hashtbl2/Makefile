DOCDIR = ../../docs
DOXYFILE = hashtbl2.doxygen

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

default : libhashtbl2.a

all : libhashtbl2.a

test :

install : libhashtbl2.a hashtbl2.h ht2_GeneralHashFunctions.h

	cp libhashtbl2.a $(LIBDIR)
	cp hashtbl2.h $(INCDIR)
	cp ht2_GeneralHashFunctions.h $(INCDIR)

so : libhashtbl2.so.1.0 

libhashtbl2.so.1.0 : $(OBJECTS)
	$(LD) $(LDFLAGS) -o libhashtbl2.so.1.0 $(OBJECTS)

hashtbl2.o : hashtbl2.c hashtbl2.h
	${CC} ${CFLAGS} -c -o hashtbl2.o hashtbl2.c

GeneralHashFunctions.o : GeneralHashFunctions.c ht2_GeneralHashFunctions.h
	${CC} ${CFLAGS} -c -o GeneralHashFunctions.o GeneralHashFunctions.c

libhashtbl2.a : $(OBJECTS)
	$(AR) $(ARFLAGS) libhashtbl2.a $(OBJECTS)
	cp hashtbl2.h ../include/

clean :
	rm *.o

cleanall :
	rm *.o libhashtbl2.a

docs :
	@if [ ! -d $(DOCDIR) ]; then mkdir $(DOCDIR); fi; \
	doxygen $(DOXYFILE)
