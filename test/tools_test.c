/*
 * tools_test.c
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
#include "../src/tools.h"

/*
 * Print usage information and exit program
 */
static void usage(void) {
    fprintf(stderr, "usage: tools_test\n");
    printf("\nTools Test Failed\n");
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
        if (pledge("stdio", NULL) == -1) {
            err(1, "pledge");
        }
    }

#endif
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
}

/*
 * Main
 */
int main(int argc, char * argv[]) {
    (void)initialize();
    //(void)validate_args(argc, argv);

    unsigned int expected = 5;
    unsigned int result = getBits(0, 2, 5);

    if (expected != result) {
        printf("\nTools Test Failed\n");
        return 1;
    } else {
        printf("\nTools Test Passed\n");
        return 0;
    }
}
