CC	:= gcc

INCDIR	:= $(HOME)/include/
LIBDIR	:= $(HOME)/lib/

CFLAGS	:= -I. -O2 -Wall -D_LARGEFILE64_SOURCE
LDFLAGS := -lclntsh -lecpg -ljdatastructs
AR      := ar -r
PROCFLAGS := parse=FULL include=. include=$(INCDIR)
# no optimization for c files generated from .pc files
PC_CFLAGS	:= -I. -Wall -D_LARGEFILE64_SOURCE
HEADER_FILES := $(wildcard *.h)

PROC_FILES := $(filter-out ffutils_test.c, $(wildcard *.pc))
PROC_CFILES := $(patsubst %.pc,%.c,$(PROC_FILES))
PROC_OBJECTS := $(patsubst %.pc,%.o,$(PROC_FILES))

C_FILES := $(wildcard *.c)
OBJECTS := $(patsubst %.c, %.o, $(C_FILES) $(PROC_CFILES))

default : libffutils.a

test : libffutils.a
	$(CC) -g -L. -o ffutils_test ffutils_test.c -lffutils $(LDFLAGS)
	./ffutils_test
	rm ffutils_test

install : libffutils.a $(HEADER_FILES)
	cp libffutils.a $(LIBDIR)
	cp $(HEADER_FILES) $(INCDIR)

ffutils.o : ffutils.c ffutils.h
	${CC} ${CFLAGS} -c -o ffutils.o ffutils.c

libffutils.a : $(OBJECTS)
	${AR} libffutils.a $(OBJECTS)

$(PROC_CFILES) : $(PROC_FILES)
	proc  $(PROCFLAGS) iname=$<

$(PROC_OBJECTS) : $(PROC_CFILES)
	$(CC) $(PC_CFLAGS) -c $<

clean :
	rm *.o $(PROC_CFILES)

cleanall :
	rm *.o libffutils.a $(PROC_CFILES)

docs : $(HEADER_FILES) ffutils.doxygen
	doxygen ffutils.doxygen

docs-install : docs
	cp docs/html/* ~/public_html/lib/c/ffutils/
