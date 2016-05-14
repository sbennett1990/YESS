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
#ifdef __OpenBSD__
    // pledge(2) only works on 5.9 or higher
    struct utsname name;

    if (uname(&name) != -1 && strncmp(name.release, "5.8", 3) > 0) {
        if (pledge("stdio rpath", NULL) == -1) {
            err(1, "pledge");
        }
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
 * Check if the file ends in ".yo"
 *
 * Parameters:
 *     *fileName    the string to check
 *
 * Return true if file ends in ".yo"; false otherwise
 */
static bool valid_file_name(char * fileName) {
    int len = strlen(fileName);

    if (fileName[len - 1] == 'o'
        && fileName[len - 2] == 'y'
        && fileName[len - 3] == '.') {
        return TRUE;
    } else {
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
    if (!valid_file_name(fileName)) {
        printf("\ninvalid file name");
        usage(); /* EXIT */
    }
}

/*
 * Main
 */
int main(int argc, char * argv[]) {
    (void)initialize();
    (void)validate_args(argc, argv);

    /*
     * Load the file
     * Terminate the program if there is a problem loading
     */
    if (!(load(argv[1]))) {
        dumpMemory();
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
