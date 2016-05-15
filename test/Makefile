#
# Makefile for Y86 Simulator (YESS) Tests
#

CC = gcc

CFLAGS = -std=c99 -DTEST
CFLAGS += -O2
#CFLAGS += -g

#CFLAGS = -Wno-unused-parameter -Wformat \
# -fstack-protector -Wformat-security -Wformat-nonliteral \
# -Wno-deprecated -Winit-self -Wpointer-arith -fno-exceptions

LDFLAGS = -Wl,-O1 -Wl,--no-undefined
LDFLAGS += -Wl,-z,relro -Wl,-z,now

SRC = ../src
MOCKS = $(SRC)/mocks

#
# Test Executables
#

tools-test: strtonum.o tools.o tools-test.o
	$(CC) $(LDFLAGS) strtonum.o tools.o tools-test.o -o tools-test

loader-test: strtonum.o tools.o memory_mock.o loader.o loader-test.o
	$(CC) $(LDFLAGS) strtonum.o tools.o memory_mock.o loader.o loader-test.o -o loader-test

#
# Regular Source Files (with mocks)
#

strtonum.o: $(SRC)/strtonum.h
	$(CC) $(CFLAGS) -c $(SRC)/strtonum.c -o strtonum.o

tools.o: $(SRC)/bool.h $(SRC)/strtonum.h $(SRC)/tools.h
	$(CC) $(CFLAGS) -c $(SRC)/tools.c -o tools.o

loader.o: $(SRC)/bool.h $(SRC)/loader.h $(MOCKS)/memory_mock.h $(SRC)/tools.h
	$(CC) $(CFLAGS) -c $(SRC)/loader.c -o loader.o

memory.o: $(SRC)/bool.h $(SRC)/memory.h $(SRC)/tools.h
	$(CC) $(CFLAGS) -c $(SRC)/memory.c -o memory.o

registers.o: $(SRC)/bool.h $(SRC)/registers.h $(SRC)/tools.h
	$(CC) $(CFLAGS) -c $(SRC)/registers.c -o registers.o

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

main.o: $(SRC)/bool.h $(SRC)/tools.h $(SRC)/memory.h $(SRC)/dump.h \
        $(SRC)/loader.h $(SRC)/forwarding.h $(SRC)/fetchStage.h \
        $(SRC)/decodeStage.h $(SRC)/executeStage.h $(SRC)/memoryStage.h \
        $(SRC)/writebackStage.h
	$(CC) $(CFLAGS) -c $(SRC)/main.c -o main.o

#
# Mock Files
#

memory_mock.o: $(SRC)/bool.h $(SRC)/tools.h $(MOCKS)/memory_mock.h
	$(CC) $(CFLAGS) -c $(MOCKS)/memory_mock.c -o memory_mock.o

#
# Test Files
#

loader-test.o: $(SRC)/bool.h $(SRC)/loader.h
	$(CC) $(CFLAGS) -c loader-test.c -o loader-test.o

tools-test.o: $(SRC)/bool.h $(SRC)/tools.h
	$(CC) $(CFLAGS) -c tools-test.c -o tools-test.o

clean:
	rm -f *.o
	rm *-test.exe
