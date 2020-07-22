
PROG=       yess
SRCS=       decodeStage.c dump.c executeStage.c fetchStage.c loader.c \
            logger.c main.c memory.c memoryStage.c registers.c \
            strtonum.c tools.c writebackStage.c

NOMAN=      noman

CC=         gcc

# make -DDEBUG will set -g and -O0
.if defined(DEBUG)
DEBUG=      -g
CFLAGS=     -O0 ${PIPE} ${DEBUG}
.else
CFLAGS=     -O1 ${PIPE}
.endif

CFLAGS+=    -std=c99 -Wall
CFLAGS+=    -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+=    -Wmissing-declarations
CFLAGS+=    -Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+=    -Wsign-compare

BINDIR=     ~/bin

install:
	install -b -o scott -g scott ${PROG} ${BINDIR}/${PROG}

.include <bsd.prog.mk>
