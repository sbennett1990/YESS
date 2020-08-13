/*
 * tools_test.c
 */

/* pledge(2) the program on OpenBSD */
#ifdef __OpenBSD__
#include <sys/utsname.h>
#include <unistd.h>
#endif

#include <err.h>
#include <stdio.h>
#include <string.h>

#include "tools.h"

static void printbuff(int * buff, size_t length);
static int test_clearBuffer(void);
static int test_getBits(int low, int high, unsigned int source, unsigned int expected);


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
 * Main
 */
int main(int argc, char * argv[]) {
    (void)initialize();

    test_getBits(0, 2, 5, 5);
    test_getBits(31, 31, 0, 0);
    test_getBits(31, 31, -1, 1);

    test_clearBuffer();

    return 0;
}

void printbuff(int * buff, size_t length) {
    for (size_t i = 0; i < length; i++) {
        printf("[%2d] ", buff[i]);
    }
}

int test_getBits(int low, int high, unsigned int source, unsigned int expected) {
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

int test_clearBuffer() {
    printf("\nTesting clearBuffer()");
    int buff[5] = { 13, 2, 43, 4, 75 };
    size_t length = sizeof(buff) / sizeof(int);

    printf("\nTest Buffer:    ");
    printbuff(buff, length);
    clearBuffer((char *) buff, sizeof(buff));
    //printf("\nExpected: %5d\nResult: %7d", expected, result);
    printf("\nCleared Buffer: ");
    printbuff(buff, length);
    printf("\n");

    if (length > 0) {
        if (buff[4] != 0) {
            printf("\nclearBuffer() Test Failed\n");
            return 0;
        } else {
            printf("\nclearBuffer() Test Passed\n");
            return 1;
        }
    }

    return 0;
}
