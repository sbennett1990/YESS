/*
 * loader_test.c
 */

/* pledge(2) the program on OpenBSD */
#ifdef __OpenBSD__
#include <sys/utsname.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/bool.h"
#include "../src/dump.h"
#include "../src/memory.h"

/*
 * Print usage information and exit program
 */
static void usage(void) {
    fprintf(stderr, "usage: dump_test <test_file>.yo\n");
    printf("\nDump Test Failed\n");
    exit(EXIT_FAILURE);
}

/*
 *
 */
static void initialize(void) {
#ifdef __OpenBSD__
    /* pledge(2) only works on 5.9 or higher */
    struct utsname name;

    if (uname(&name) != -1 && strncmp(name.release, "5.8", 3) > 0) {
        if (pledge("stdio rpath", NULL) == -1) {
            err(1, "pledge");
        }
    }

#endif
}

/*
 * Validate that the file name ends in ".yo".
 *
 * Parameters:
 *     *fileName    pointer to the string to check
 *
 * Return true if file ends in ".yo"; false otherwise
 */
static bool validatefilename(char * fileName) {
    int len = (int) strlen(fileName);

    if (len < 3) {
        return FALSE;
    }

    if (fileName[len - 1] == 'o'
        && fileName[len - 2] == 'y'
        && fileName[len - 3] == '.') {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
 * Main
 */
int main(int argc, char ** argv) {
    (void)initialize();
    //(void)validate_args(argc, argv);

    (void)clearMemory();
    (void)dumpMemory();
    
    return 0;
}
