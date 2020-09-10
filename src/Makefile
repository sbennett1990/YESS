
PROG=       yess
SRCS=       decodeStage.c dump.c executeStage.c fetchStage.c loader.c \
            logger.c main.c memLoader.c memory.c memoryStage.c registers.c \
            strtonum.c tools.c writebackStage.c

NOMAN=      noman

# make -DDEBUG will set -g and -O0
.if defined(DEBUG)
DEBUG=      -g
CFLAGS=     -O0 ${PIPE} ${DEBUG}
.endif

.ifmake afl
#CC=         /path/to/afl-clang
CFLAGS+=    -DAFL_TEST
.endif

CFLAGS+=    -std=c99 -Wall
CFLAGS+=    -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+=    -Wmissing-declarations
CFLAGS+=    -Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+=    -Wsign-compare

BINDIR=     ~/bin

install:
	install -b -m 0740 ${PROG} ${BINDIR}/${PROG}

# make an instrumented binary to fuzz with AFL
afl: ${PROG} ${PROGS} _SUBDIRUSE

.include <bsd.prog.mk>
