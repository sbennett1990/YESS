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

static void printbuff(int * buff) {
    size_t length = sizeof(buff);

    for (size_t i = 0; i < length; i++) {
        printf("[%d] ", buff[i]);
    }
}

static int test_getBits(int low, int high, unsigned int source, unsigned int expected) {
    printf("\nTesting getBits(%d, %d, %d)", low, high, source);
    unsigned int result = getBits(low, high, source);
    printf("\nExpected: %5d\nResult: %7d", expected, result);

    if (expected != result) {
        printf("\ngetBits() Test Failed\n");
        return 0;
    } else {
        printf("\ngetBits() Test Passed\n");
        return 1;
    }
}

static int test_clearBuffer() {
    printf("\nTesting clearBuffer()");
    int buff[5] = { 1, 2, 3, 4, 5 };
    size_t length = sizeof(buff);

    printf("\nTest Buffer: ");
    printbuff(buff);
    clearBuffer((char *) buff, length);
    //printf("\nExpected: %5d\nResult: %7d", expected, result);
    printf("\nCleared Buffer: ");
    printbuff(buff);
    printf("\n");
    clearBuffer((char *) buff, length);

    if (length > 0) {
        if (buff[0] != 0) {
            printf("\nclearBuffer() Test Failed\n");
            return 0;
        } else {
            printf("\nclearBuffer() Test Passed\n");
            return 1;
        }
    }

    return 0;
}

/*
 * Main
 */
int main(int argc, char * argv[]) {
    (void)initialize();
    //(void)validate_args(argc, argv);

    test_getBits(0, 2, 5, 5);
    test_getBits(31, 31, 0, 0);
    test_getBits(31, 31, -1, 1);

    test_clearBuffer();

    return 0;
}
