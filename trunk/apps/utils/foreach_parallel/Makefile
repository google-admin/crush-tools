EXECUTABLE := foreach_parallel
DESCRIPTION := processes a block of shell in parallel for multiple inputs
VERSION = 0.1

DISTFILE = $(EXECUTABLE)-$(VERSION).tgz
# resources like README, COPYING, etc., which should be included in
# the distribution file
RESOURCES := 

WORKDIR := $(shell pwd)
CC := $(shell which gcc)

MAINGENERATOR := $(shell which cgener)
ARGSTAB := args.tab
CFLAGS := -O2 -Wall
DBCFLAGS := $(CFLAGS) -g -DDEBUG
LINKFLAGS :=

GEN_FILES := main.c usage.c
GEN_OBJECTS := $(patsubst %.c,%.o,$(GEN_FILES))
CFILES := $(filter-out $(GEN_FILES), $(wildcard *.c))
CHEADERS :=$(wildcard *.h)
OBJECTS := $(patsubst %.c,%.o,$(CFILES))
DEPS := $(patsubst %.o,%.d,$(OBJECTS))

.PHONY : debug deps clean dist

default : $(EXECUTABLE)

deps :  $(OBJECTS) $(GEN_OBJECTS)

debug ::
	$(CC) $(DBCFLAGS) -o $(EXECUTABLE) $(CFILES) $(LINKFLAGS)

clean ::
	rm -f *.o
	rm -f *.d
	rm -f $(GEN_FILES)

dist : $(DISTFILE)

$(DISTFILE) : $(CFILES) $(GEN_FILES) $(CHEADERS) $(RESOURCES)
	@ls $(CFILES) $(GEN_FILES) $(CHEADERS) $(RESOURCES) | sed s:^:$(EXECUTABLE)-$(VERSION)/: >MANIFEST
	@(cd ..; ln -s $(WORKDIR) $(EXECUTABLE)-$(VERSION))
	(cd ..; tar -czvf $(WORKDIR)/$(DISTFILE) `cat $(WORKDIR)/MANIFEST`)
	@(cd ..; rm $(EXECUTABLE)-$(VERSION))

$(GEN_OBJECTS) : $(GEN_FILES)

$(GEN_FILES) : $(ARGSTAB)
	$(MAINGENERATOR) -a $(ARGSTAB)

$(EXECUTABLE) : $(GEN_OBJECTS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(GEN_OBJECTS) $(OBJECTS) $(LINKFLAGS)


