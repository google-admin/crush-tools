SHELL := $(shell which bash)

DOCDIR = ../docs
DOXYFILE = jdatastructs.doxygen

CC	:= gcc
LD	:= ld

SUBDIRS := linklist queue bstree hashtbl

HEADERS := $(shell for d in $(SUBDIRS); do ls $$d/*.h; done)
SOURCES := $(shell for d in $(SUBDIRS); do ls $$d/*.c; done)
OBJECTS := $(shell for d in $(SUBDIRS); do ls $$d/*.o; done)

CFLAGS	+= -I. -I../include -L../lib -O2 -Wall -fPIC
LDFLAGS := -dy -G -z text 
AR      := ar
ARFLAGS := rcs
INCDIR	:= /usr/local/devprac/include/
LIBDIR	:= /usr/local/devprac/lib/

.PHONY : all docs

RECURSIVE_TARGETS := all-recursive

default : all

all :
	$(MAKE) all-recursive
	$(MAKE) libjdatastructs.a

test :

install : libjdatastructs.a $(HEADERS)
	cp $(HEADERS) $(INCDIR)
	cp libjdatastructs.a $(LIBDIR)

so : libjdatastructs.so.1.0 

libjdatastructs.so.1.0 : $(OBJECTS)
	$(LD) $(LDFLAGS) -o libjdatastructs.so.1.0 $(OBJECTS)

libjdatastructs.a : $(OBJECTS)
	$(AR) $(ARFLAGS) libjdatastructs.a $(OBJECTS)

clean :
	-rm $(OBJECTS)
	-rm libjdatastructs.a

cleanall :
	$(MAKE) clean

docs :
	@if [ ! -d $(DOCDIR) ]; then mkdir $(DOCDIR); fi; \
	doxygen $(DOXYFILE)

# make some target in the subdirectories
$(RECURSIVE_TARGETS):
	@target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
		echo "Making $$target in $$subdir"; \
		(cd $$subdir && $(MAKE) $$target) || \
			case "$$amf" in \
				*=*) exit 1;; \
				*k*) fail=yes;; \
				*) exit 1;; \
			esac; \
	done; \
	test -z "$$fail"
