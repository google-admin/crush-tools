# doing this causes a preprocessor warning :|
# DEFINES=-DMAX_TRUNC_FIELDS=16

DEFINES=

INSTALLDIR=/home/jhinds/bin/
INCLUDE=/home/jhinds/include

BINNAME=truncfield

SRCFILES=./truncfield.c

CFLAGS=-ansi -Wall -O2


build ::; gcc ${CFLAGS} -I${INCLUDE} ${DEFINES} -o ${BINNAME} ${SRCFILES}

install ::; cp ${BINNAME} ${INSTALLDIR}
