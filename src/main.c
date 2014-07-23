/*
 * File:   main.c
 * Author: Scott Bennett
 */

#include <stdio.h>
#include <stdlib.h>

#include "bool.h"
#include "tools.h"
#include "memory.h"
#include "dump.h"
#include "loader.h"
#include "status.h"
#include "control.h"
#include "forwarding.h"
#include "fetchStage.h"
#include "decodeStage.h"
#include "executeStage.h"
#include "memoryStage.h"
#include "writebackStage.h"

// Prototypes for "private" functions
static void initialize(void);

int main(int argc, char * argv[]) {
    (void)initialize();
    
    // Load the file
    // Terminate the program if there is a problem loading
    if (!(load(argc, argv))) {
        dumpMemory();
        (void)exit(EXIT_FAILURE);
    }
    
    int clockCount = 0;
    bool stop = FALSE;
    forwardType forward;
    statusType status;
    controlType control;

    // Each loop iteration is 1 clock cycle
    while (!stop) {
        stop = writebackStage(&forward, &status);
        (void)memoryStage(&forward, &status, &control);
        (void)executeStage(&forward, status, &control);
        (void)decodeStage(forward, &control);
        (void)fetchStage(forward, control);
        clockCount++;
    }
    
    printf("\nTotal clock cycles = %d\n", clockCount);
}

/*
 * Initialize the "memory" and pipelined registers in
 * the Y86 processor. Also initialize the function
 * pointer array used in executeStage.c
 */
void initialize() {
    // Initialize function pointer array here
    (void)initFuncPtrArray();

    (void)clearMemory();
    (void)clearFregister();
    (void)clearDregister();
    (void)clearEregister();
    (void)clearMregister();
    (void)clearWregister();
}

