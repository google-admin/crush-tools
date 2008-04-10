ifeq (,$(PREFIX))
PATH_PREFIX = ~
else
PATH_PREFIX = $(PREFIX)
endif

HTML_DOC_PATH = $(PATH_PREFIX)/public_html/lib/c/qsort_helper/
VERSION = 1.0

CC	:= gcc

CFLAGS	:= -I. -O2 -Wall -fPIC
LDFLAGS :=
AR      := ar -r

INCDIR	:= $(PATH_PREFIX)/include/
LIBDIR	:= $(PATH_PREFIX)/lib/

SHARED_LIB = libqsort_helper.so.$(VERSION)

.PHONY : test install

default : libqsort_helper.a $(SHARED_LIB)

# remove -fPIC from CFLAGS
test : CFLAGS = -I. -L. -O2 -Wall
test : $(SHARED_LIB) qsort_helper_test.c
	@(LD_LIBRARY_PATH=.:$(LD_LIBRARY_PATH); \
	if [ ! -e libqsort_helper.so ]; then \
		ln -s $(SHARED_LIB) libqsort_helper.so; \
	fi; \
	$(CC) -I. -L. -o qsort_helper_test -lqsort_helper qsort_helper_test.c; \
	./qsort_helper_test && \
	rm ./qsort_helper_test libqsort_helper.so )

install : $(SHARED_LIB) libqsort_helper.a qsort_helper.h
	install -D -c libqsort_helper.a $(LIBDIR)/libqsort_helper.a
	install -D -c $(SHARED_LIB) $(LIBDIR)/$(SHARED_LIB)
	if [ -e $(LIBDIR)/libqsort_helper.so ]; then rm $(LIBDIR)/libqsort_helper.so; fi
	ln -s $(SHARED_LIB) $(LIBDIR)/libqsort_helper.so
	install -D -c qsort_helper.h $(INCDIR)/qsort_helper.h

qsort_helper.o : qsort_helper.c qsort_helper.h
	$(CC) $(CFLAGS) -c -o qsort_helper.o qsort_helper.c

libqsort_helper.a : qsort_helper.o
	${AR} libqsort_helper.a qsort_helper.o

$(SHARED_LIB) : qsort_helper.o
	$(CC) -shared -Wl,-soname,$(SHARED_LIB) -o $(SHARED_LIB) qsort_helper.o

clean :
	rm *.o

cleanall :
	rm *.o $(SHARED_LIB) libqsort_helper.a

docs : qsort_helper.c qsort_helper.h qsort_helper.doxygen
	doxygen qsort_helper.doxygen

docs-install : docs
	cp docs/html/* $(HTML_DOC_PATH)
