CC = gcc -march=opteron 

yess: tools.o registers.o memory.o fetchStage.o decodeStage.o executeStage.o \
      memoryStage.o writebackStage.o loader.o dump.o main.o
	$(CC) tools.o registers.o memory.o decodeStage.o executeStage.o \
    fetchStage.o memoryStage.o writebackStage.o loader.o dump.o main.o -o yess
registers.o: bool.h registers.h tools.h

memory.o: bool.h memory.h tools.h

fetchStage.o: bool.h tools.h fetchStage.h instructions.h memory.h \
              registers.h forwarding.h decodeStage.h

decodeStage.o: bool.h tools.h forwarding.h decodeStage.h executeStage.h \
               registers.h instructions.h

executeStage.o: bool.h executeStage.h tools.h instructions.h \
                memoryStage.h registers.h

memoryStage.o: bool.h memoryStage.h tools.h forwarding.h writebackStage.h

writebackStage.o: bool.h tools.h dump.h forwarding.h writebackStage.h \
                  executeStage.h decodeStage.h instructions.h

dump.o: bool.h dump.h forwarding.h fetchStage.h decodeStage.h executeStage.h \
        memoryStage.h writebackStage.h registers.h memory.h

loader.o: bool.h loader.h memory.h tools.h

main.o: bool.h tools.h memory.h dump.h loader.h forwarding.h fetchStage.h \
        decodeStage.h executeStage.h memoryStage.h writebackStage.h

tools.o: bool.h tools.h

clean:
	rm -f *.o


