# Makefile for yess regression tests

PROGS=     memory-test registers-test tools-test

SRCS_memory-test=    memory-test.c
SRCS_registers-test= registers-test.c
SRCS_tools-test=     tools-test.c

HEADERS=   bool.h dump.h loader.h logger.h memory.h registers.h strtonum.h \
           tools.h
FILES=     dump.c loader.c logger.c memory.c registers.c strtonum.c tools.c

SRCS_memory-test+=    ${FILES}
SRCS_registers-test+= ${FILES}
SRCS_tools-test+=     ${FILES}

NOMAN=     noman

CFLAGS+=   -std=c99 -Wall
CFLAGS+=   -DTEST

SRCDIR=    ../src

# Redefine 'all' target; do beforebuild prior to the rest
all: beforebuild ${PROG} ${PROGS} _SUBDIRUSE

# Just copy yess sources here to build tests
beforebuild:
	@echo 'preparing to build regression tests...'
.for s in ${HEADERS}
	cp ${SRCDIR}/$s .
.endfor
.for s in ${FILES}
	cp ${SRCDIR}/$s .
.endfor

cleandepends: clean
	rm -f ${HEADERS} ${FILES}

.include <bsd.prog.mk>
