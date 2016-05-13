/*
 * File:   main.c
 * Author: Scott Bennett
 */

// pledge(2) the program on OpenBSD
#ifdef __OpenBSD__
#include <sys/utsname.h>
#include <string.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

static void initialize(void);

int main(int argc, char * argv[]) {
    (void)initialize();
    
    /*
     * Load the file
     * Terminate the program if there is a problem loading
     */
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

    return 0;
}

/*
 * Initialize the "memory" and pipelined registers in
 * the Y86 processor. Also initialize the function
 * pointer array used in executeStage.c
 */
void initialize() {
#ifdef __OpenBSD__
    // pledge(2) only works on 5.9 or higher
    struct utsname name;
    if (uname(&name) != -1 && strncmp(name.release, "5.8", 3) > 0) {
        if (pledge("stdio rpath", NULL) == -1) {
            err(1, "pledge");
        }
    }
#endif
    // Initialize function pointer array here
    (void)initFuncPtrArray();

    (void)clearMemory();
    (void)clearFregister();
    (void)clearDregister();
    (void)clearEregister();
    (void)clearMregister();
    (void)clearWregister();
}

