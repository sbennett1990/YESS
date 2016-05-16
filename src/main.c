/*
 * main.c
 */

/* pledge(2) the program on OpenBSD */
#ifdef __OpenBSD__
#include <sys/utsname.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
static void usage(void) {
    fprintf(stderr, "usage: yess [-dv] -f <filename>.yo\n");
    exit(EXIT_FAILURE);
}

/*
 * Initialize the program. This includes pledging the program on OpenBSD and
 * setting up logging. Called AFTER parsing the options.
 *
 * Parameters:
 *     verbosity    how verbose logging output should be
 */
static void initialize(int verbosity) {
    log_init(verbosity, 0);
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
}

/*
 * Validate that the file name ends in ".yo".
 *
 * Parameters:
 *     *filename    file name to check
 *
 * Return true if file ends in ".yo"; false otherwise
 */
static bool validatefilename(char * filename) {
    int len = (int) strlen(filename);

    if (len < 3) {
        log_warn("filename too short");
        return FALSE;
    }

    if (filename[len - 1] == 'o'
        && filename[len - 2] == 'y'
        && filename[len - 3] == '.') {
        log_debug("filename valid");
        return TRUE;
    } else {
        log_warn("filename not valid");
        return FALSE;
    }
}

/*
 * Set up the "memory" and pipelined registers for the Y86 processor and the
 * function pointer array used in executeStage.c
 */
static void setupyess() {
    /* initialize function pointer array */
    (void)initFuncPtrArray();

    (void)clearMemory();
    (void)clearFregister();
    (void)clearDregister();
    (void)clearEregister();
    (void)clearMregister();
    (void)clearWregister();
}

/*
 * Main
 */
int main(int argc, char * argv[]) {
    int ch;
    bool dflag = FALSE;
    bool vflag = FALSE;
    int verbosity = 0;
    char * sourcefile;

    while ((ch = getopt(argc, argv, "df:v")) != -1) {
        switch (ch) {
            case 'd':
                dflag = TRUE;
                break;

            case 'f':
                sourcefile = optarg;
                break;

            case 'v':
                vflag = TRUE;
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
    (void)initialize(verbosity);

    if (!validatefilename(sourcefile)) {
        usage(); /* EXIT */
    }

    /* set up the 'processor' */
    (void)setupyess();

    /* load the file; terminate if there is a problem */
    if (!load(sourcefile)) {
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

    /* each loop iteration is 1 clock cycle */
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
