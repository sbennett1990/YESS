/*
 * File:   main.c
 * Author: Scott Bennett
 */

// pledge(2) the program on OpenBSD
#ifdef __OpenBSD__
#include <sys/utsname.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bool.h"
#include "tools.h"
#include "logger.h"
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

/*
 * Print usage information and exit program
 */
static void usage(void) {
    fprintf(stderr, "usage: yess <filename>.yo\n");
    exit(EXIT_FAILURE);
}

/*
 * Initialize the program. This includes setting up the "memory" and pipelined
 * registers for the Y86 processor, and the function pointer array used in
 * executeStage.c
 */
static void initialize(void) {
    log_init(2, 0);
    log_debug("initializing YESS...");
#ifdef __OpenBSD__
    // pledge(2) only works on 5.9 or higher
    struct utsname name;

    if (uname(&name) != -1 && strncmp(name.release, "5.8", 3) > 0) {
        const char * promises = "stdio rpath";

        if (pledge(promises, NULL) == -1) {
            err(1, "pledge");
        }

        log_info("pledge(2)'d with %s", promises);
    }

#endif
    // Initialize function pointer array
    (void)initFuncPtrArray();

    (void)clearMemory();
    (void)clearFregister();
    (void)clearDregister();
    (void)clearEregister();
    (void)clearMregister();
    (void)clearWregister();
}

/*
 * Validate that the file name ends in ".yo".
 *
 * Parameters:
 *     *fileName    pointer to the string to check
 *
 * Return true if file ends in ".yo"; false otherwise
 */
bool validatefilename(char * fileName) {
    int len = (int) strlen(fileName);

    if (len < 3) {
        log_warn("filename too short");
        return FALSE;
    }

    if (fileName[len - 1] == 'o'
        && fileName[len - 2] == 'y'
        && fileName[len - 3] == '.') {
        log_debug("filename valid");
        return TRUE;
    } else {
        log_warn("filename not valid");
        return FALSE;
    }
}

/*
 * Validate that the correct number of arguments are provided and that the
 * second argument is a valid file name. If the arguments are invalid,
 * usage info will be printed and the program will exit.
 */
static void validate_args(int argc, char * argv[]) {
    if (argc != 2) {
        usage(); /* EXIT */
    }

    char * fileName = argv[1];

    // make sure file name is valid
    if (!validatefilename(fileName)) {
        usage(); /* EXIT */
    }

    log_debug("arguments valid");
}

/*
 * Main
 */
int main(int argc, char * argv[]) {
    (void)initialize();
    (void)validate_args(argc, argv);

    /* Load the file; terminate if there is a problem */
    if (!(load(argv[1]))) {
        dumpMemory();
        log_warn("error loading the file");
        log_debug("exiting");
        return 1; /* EXIT */
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
