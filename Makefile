#
# Makefile for Y86 Simulator (YESS)
#

CC = gcc

CFLAGS = -std=c99 -Wall
CFLAGS += -g

LDFLAGS = -Wl,-O1 -Wl,--no-undefined
LDFLAGS += -Wl,-z,relro -Wl,-z,now

SRC = ./src

yess: strtonum.o logger.o tools.o registers.o memory.o fetchStage.o \
      decodeStage.o executeStage.o memoryStage.o writebackStage.o loader.o \
      dump.o main.o
	$(CC) $(LDFLAGS) strtonum.o logger.o tools.o registers.o memory.o fetchStage.o \
    decodeStage.o executeStage.o memoryStage.o writebackStage.o loader.o \
    dump.o main.o -o yess

registers.o: $(SRC)/bool.h $(SRC)/registers.h $(SRC)/tools.h
	$(CC) $(CFLAGS) -c $(SRC)/registers.c -o registers.o

memory.o: $(SRC)/bool.h $(SRC)/memory.h $(SRC)/tools.h
	$(CC) $(CFLAGS) -c $(SRC)/memory.c -o memory.o

fetchStage.o: $(SRC)/bool.h $(SRC)/tools.h $(SRC)/fetchStage.h \
              $(SRC)/instructions.h $(SRC)/memory.h $(SRC)/registers.h \
              $(SRC)/forwarding.h $(SRC)/decodeStage.h
	$(CC) $(CFLAGS) -c $(SRC)/fetchStage.c -o fetchStage.o

decodeStage.o: $(SRC)/bool.h $(SRC)/tools.h $(SRC)/forwarding.h \
               $(SRC)/decodeStage.h $(SRC)/executeStage.h $(SRC)/registers.h \
               $(SRC)/instructions.h
	$(CC) $(CFLAGS) -c $(SRC)/decodeStage.c -o decodeStage.o

executeStage.o: $(SRC)/bool.h $(SRC)/executeStage.h $(SRC)/tools.h \
                $(SRC)/instructions.h $(SRC)/memoryStage.h $(SRC)/registers.h
	$(CC) $(CFLAGS) -c $(SRC)/executeStage.c -o executeStage.o

memoryStage.o: $(SRC)/bool.h $(SRC)/memoryStage.h $(SRC)/tools.h \
               $(SRC)/forwarding.h $(SRC)/writebackStage.h
	$(CC) $(CFLAGS) -c $(SRC)/memoryStage.c -o memoryStage.o

writebackStage.o: $(SRC)/bool.h $(SRC)/tools.h $(SRC)/dump.h $(SRC)/forwarding.h \
                  $(SRC)/writebackStage.h $(SRC)/executeStage.h \
                  $(SRC)/decodeStage.h $(SRC)/instructions.h
	$(CC) $(CFLAGS) -c $(SRC)/writebackStage.c -o writebackStage.o

dump.o: $(SRC)/bool.h $(SRC)/dump.h $(SRC)/forwarding.h $(SRC)/fetchStage.h \
        $(SRC)/decodeStage.h $(SRC)/executeStage.h $(SRC)/memoryStage.h \
        $(SRC)/writebackStage.h $(SRC)/registers.h $(SRC)/memory.h
	$(CC) $(CFLAGS) -c $(SRC)/dump.c -o dump.o

loader.o: $(SRC)/bool.h $(SRC)/loader.h $(SRC)/logger.h $(SRC)/memory.h \
          $(SRC)/tools.h
	$(CC) $(CFLAGS) -c $(SRC)/loader.c -o loader.o

main.o: $(SRC)/bool.h $(SRC)/tools.h $(SRC)/logger.h $(SRC)/memory.h $(SRC)/dump.h \
        $(SRC)/loader.h $(SRC)/forwarding.h $(SRC)/fetchStage.h \
        $(SRC)/decodeStage.h $(SRC)/executeStage.h $(SRC)/memoryStage.h \
        $(SRC)/writebackStage.h
	$(CC) $(CFLAGS) -c $(SRC)/main.c -o main.o

tools.o: $(SRC)/bool.h $(SRC)/strtonum.h $(SRC)/tools.h
	$(CC) $(CFLAGS) -c $(SRC)/tools.c -o tools.o

strtonum.o: $(SRC)/strtonum.h
	$(CC) $(CFLAGS) -c $(SRC)/strtonum.c -o strtonum.o

logger.o: $(SRC)/logger.h
	$(CC) $(CFLAGS) -c $(SRC)/logger.c -o logger.o

clean:
	rm -f *.o
	rm yess


