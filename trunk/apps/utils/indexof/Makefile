EXECUTABLE := indexof
DESCRIPTION := prints the column number matching a given header, or 0 if not found.

CC := $(shell which gcc)

MAINGENERATOR := $(shell which cgen)
ARGSTAB := args.tab

CFLAGS := -O2 -Wall -D_GNU_SOURCE
DBCFLAGS := $(CFLAGS) -g -DDEBUG
LINKFLAGS := -lffutils

GEN_FILES := main.c usage.c
GEN_OBJECTS := $(patsubst %.c,%.o,$(GEN_FILES))
CFILES := $(filter-out $(GEN_FILES), $(wildcard *.c))
OBJECTS := $(patsubst %.c,%.o,$(CFILES))
DEPS := $(patsubst %.o,%.d,$(OBJECTS))


.PHONY : debug deps clean

default : $(EXECUTABLE)

deps :  $(OBJECTS) $(GEN_OBJECTS)

debug ::
	$(CC) $(DBCFLAGS) -o $(EXECUTABLE) $(CFILES) $(LINKFLAGS)

clean ::
	rm -f *.o
	rm -f *.d
	rm -f $(GEN_FILES)

$(GEN_OBJECTS) : $(GEN_FILES)

$(GEN_FILES) : $(ARGSTAB)
	$(MAINGENERATOR) -a $(ARGSTAB)

$(EXECUTABLE) : $(GEN_OBJECTS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(GEN_OBJECTS) $(OBJECTS) $(LINKFLAGS)


