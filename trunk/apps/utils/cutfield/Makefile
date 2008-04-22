# -*- makefile -*-

ifeq (,$(PREFIX))
PATH_PREFIX = ~
else
PATH_PREFIX = $(PREFIX)
endif

EXECUTABLE := cutfield
DESCRIPTION := removes specified fields from a string-delimited data stream
VERSION = 0.1
# resources like README, COPYING, etc., which should be included in
# the distribution file
RESOURCES := 


DISTFILE = $(EXECUTABLE)-$(VERSION).tgz

WORKDIR := $(shell pwd)
CC := $(shell which gcc)

MAINGENERATOR := $(shell which cgener)
ARGSTAB := args.tab
CFLAGS := -O2 -Wall -D_GNU_SOURCE
DBCFLAGS := $(CFLAGS) -g -DDEBUG
LINKFLAGS := -lffutils -lqsort_helper

GEN_FILES := main.c usage.c
GEN_OBJECTS := $(patsubst %.c,%.o,$(GEN_FILES))
CFILES := $(filter-out $(GEN_FILES), $(wildcard *.c))
CHEADERS :=$(wildcard *.h)
OBJECTS := $(patsubst %.c,%.o,$(CFILES))
DEPS := $(patsubst %.o,%.d,$(OBJECTS))

BINDIR	:= $(PATH_PREFIX)/bin

.PHONY : debug deps clean dist install

default : $(EXECUTABLE)

deps :  $(OBJECTS) $(GEN_OBJECTS)

debug ::
	$(CC) $(DBCFLAGS) -o $(EXECUTABLE) $(CFILES) $(LINKFLAGS)

clean ::
	rm -f *.o
	rm -f *.d
	rm -f $(GEN_FILES)

dist : $(DISTFILE)

install : $(EXECUTABLE)
	install -D -c $(EXECUTABLE) $(BINDIR)/$(EXECUTABLE)


$(DISTFILE) : $(CFILES) $(GEN_FILES) $(CHEADERS) $(RESOURCES)
	@ls $(CFILES) $(GEN_FILES) $(CHEADERS) $(RESOURCES) | sed s:^:$(EXECUTABLE)-$(VERSION)/: >MANIFEST
	@(cd ..; ln -s $(WORKDIR) $(EXECUTABLE)-$(VERSION))
	(cd ..; tar -czvf $(WORKDIR)/$(DISTFILE) `cat $(WORKDIR)/MANIFEST`)
	@(cd ..; rm $(EXECUTABLE)-$(VERSION))

$(GEN_OBJECTS) : $(GEN_FILES)

$(GEN_FILES) : $(ARGSTAB)
	$(MAINGENERATOR) -a $(ARGSTAB)

$(EXECUTABLE) : $(GEN_OBJECTS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(GEN_OBJECTS) $(OBJECTS) $(LINKFLAGS) $(LDFLAGS)


