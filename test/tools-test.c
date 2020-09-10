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

    return 0;
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
