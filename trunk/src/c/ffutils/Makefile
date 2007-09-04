VERSION = 1.0
LIB_NAME = libffutils
SO_NAME = $(LIB_NAME).so.$(VERSION)
STATIC_NAME =$(LIB_NAME).a 

CC	:= gcc

INCDIR	:= $(HOME)/include/
LIBDIR	:= $(HOME)/lib/

CFLAGS	:= -I. -O2 -Wall -D_LARGEFILE64_SOURCE
LDFLAGS := 
AR      := ar -r
HEADER_FILES := $(wildcard *.h)

C_FILES := $(filter-out ffutils_test.c, $(wildcard *.c))
OBJECTS := $(patsubst %.c, %.o, $(C_FILES))

default : $(SO_NAME) $(STATIC_NAME)

test : $(STATIC_NAME)
	$(CC) -g -L. -o ffutils_test ffutils_test.c -lffutils $(LDFLAGS)
	./ffutils_test
	rm ffutils_test

install : $(STATIC_NAME) $(SO_NAME) $(HEADER_FILES)
	cp $(HEADER_FILES) $(INCDIR) && \
	cp $(SO_NAME) $(STATIC_NAME) $(LIBDIR) && \
	ln -f -s $(LIBDIR)/$(SO_NAME) $(LIBDIR)/$(LIB_NAME).so

ffutils.o : ffutils.c ffutils.h
	${CC} ${CFLAGS} -c -o ffutils.o ffutils.c

$(SO_NAME) : ffutils.o
	$(CC) -shared -Wl,-soname,$(SO_NAME) -o $(SO_NAME) ffutils.o

$(STATIC_NAME) : $(OBJECTS)
	${AR} libffutils.a $(OBJECTS)

clean :
	rm *.o

cleanall :
	rm *.o libffutils.a

docs : $(HEADER_FILES) ffutils.doxygen
	doxygen ffutils.doxygen

docs-install : docs
	cp docs/html/* ~/public_html/lib/c/ffutils/
