/*
 * main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
 * Print usage information and exit program.
 */
static void
usage(void)
{
    fprintf(stderr, "usage: yess [-duv] -f <filename>.yo\n");
    exit(EXIT_FAILURE);
}

/*
 * Set up the "memory" and pipelined registers for the Y86 processor and the
 * function pointer array used in the Execute Stage.
 */
static void
setupyess(void)
{
    /* initialize function pointer array */
    initFuncPtrArray();

    clearMemory();
    clearFregister();
    clearDregister();
    clearEregister();
    clearMregister();
    clearWregister();
}

/*
 * Main
 */
int
main(int argc, char ** argv)
{
    int ch;
    int dflag = 0;	/* debug */
    int vflag = 0;	/* verbose */
    int verbosity = 0;
    const char * sourcefile;

    while ((ch = getopt(argc, argv, "df:uv")) != -1) {
        switch (ch) {
            case 'd':
                dflag = 1;
                break;
            case 'f':
                sourcefile = optarg;
                break;
            case 'u':
                usage(); /* EXIT */
            case 'v':
                vflag = 1;
                break;
            default:
                usage();
                /* NOTREACHED */
        }
    }
    argc -= optind;
    argv += optind;

    if (argc > 0 || sourcefile == NULL) {
        usage(); /* EXIT */
    }

    if (vflag) {
        verbosity = 1;
    }

    /* max verbosity, with or without -v flag */
    if (dflag) {
        verbosity = 2;
    }

    /* done with option parsing, initialize the program */
    log_init(verbosity, 0);
    log_debug("initializing YESS...");
    initialpledge();

    /* initialize the 'processor' */
    setupyess();

    /* load 'program' file into 'memory'; terminate if there is a problem */
    if (!load(sourcefile)) {
        dumpMemory();
        log_warn("error loading the file");
        log_debug("exiting");
        return 1; /* EXIT */
    }

	/* reduce privileges */
    reduceprivileges();

	/* execute the 'program' */
    int clockCount = 0;
    int stop = 0;
    forwardType forward;
    statusType status;
    controlType control;

    while (stop != -1) {
        stop = writebackStage(&forward, &status);
        memoryStage(&forward, &status, &control);
        executeStage(&forward, status, &control);
        decodeStage(forward, &control);
        fetchStage(forward, control);
        clockCount++;	/* each loop iteration is 1 clock cycle */
    }

    printf("\nTotal clock cycles = %d\n", clockCount);
    return 0;
}
