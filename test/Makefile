# Makefile for yess regression tests

PROGS=     memory-test tools-test

SRCS_memory-test=   memory-test.c
SRCS_tools-test=    tools-test.c

HEADERS=   bool.h logger.h memory.h strtonum.h tools.h
FILES=     logger.c memory.c strtonum.c tools.c

SRCS_memory-test+=   ${FILES}
SRCS_tools-test+=    ${FILES}

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
