# Makefile for yess regression tests

PROG=      memory-test
SRCS=      memory-test.c

HEADERS=   bool.h logger.h memory.h strtonum.h tools.h
FILES=     logger.c memory.c strtonum.c tools.c

SRCS+=     ${FILES}

NOMAN=     noman

CC=        gcc

CFLAGS=    -O1 ${PIPE}
CFLAGS+=   -std=c99 -Wall

SRCDIR=    ../src

beforebuild:
.for s in ${HEADERS}
	cp ${SRCDIR}/$s .
.endfor
.for s in ${FILES}
	cp ${SRCDIR}/$s .
.endfor

cleandepends: clean
	rm -f ${HEADERS} ${FILES}

.include <bsd.prog.mk>
