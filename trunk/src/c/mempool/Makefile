PATH_PREFIX = ~
HTML_DOC_PATH = $(PATH_PREFIX)/public_html/lib/c/mempool/
VERSION = 1.0

CC	:= gcc

CFLAGS	:= -I. -O2 -Wall -fPIC
LDFLAGS :=
AR      := ar rsv

INCDIR	:= $(PATH_PREFIX)/include/
LIBDIR	:= $(PATH_PREFIX)/lib/

SO_NAME = libmempool.so.$(VERSION)

.PHONY : test install

default : libmempool.a $(SO_NAME)

# remove -fPIC from CFLAGS for compiling test app
test : CFLAGS = -I. -L. -O2 -Wall
test : $(SO_NAME) mempool_test.c
	@(LD_LIBRARY_PATH=.:$(LD_LIBRARY_PATH); \
	if [ -e libmempool.so ]; then \
		rm libmempool.so; \
	fi; \
	ln -s $(SO_NAME) libmempool.so; \
	$(CC) -L. -I. -o mempool_test mempool_test.c -lmempool; \
	./mempool_test; \
	rm ./mempool_test)

install : $(SO_NAME) libmempool.a mempool.h
	cp $(SO_NAME) $(LIBDIR)
	if [ -e $(LIBDIR)/libmempool.so ]; then rm $(LIBDIR)/libmempool.so; fi
	ln -s $(LIBDIR)/$(SO_NAME) $(LIBDIR)/libmempool.so
	cp libmempool.a $(LIBDIR)
	cp mempool.h $(INCDIR)

mempool.o : mempool.c mempool.h
	$(CC) $(CFLAGS) -c -o mempool.o mempool.c

$(SO_NAME) : mempool.o
	$(CC) -shared -Wl,-soname,$(SO_NAME) -o $(SO_NAME) mempool.o

libmempool.a : mempool.o
	${AR} libmempool.a mempool.o

clean :
	rm *.o
	if [ -e libmempool.so ]; then rm libmempool.so; fi

cleanall :
	rm *.o libmempool.a $(SO_NAME)

docs : mempool.c mempool.h mempool.doxygen
	doxygen mempool.doxygen

docs-install : docs
	cp docs/html/* $(HTML_DOC_PATH)
