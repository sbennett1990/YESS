/*  $OpenBSD: src/lib/libc/stdlib/strtonum.c,v 1.7 2013/04/17 18:40:58 tedu Exp $   */

/*
 * Copyright (c) 2004 Ted Unangst and Todd Miller
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * NAME
 * strtonum - reliably convert string value to an integer
 *
 * SYNOPSIS
 * #include "strtonum.h"
 * long
 * strtonum(const char *numstr, long minval, long maxval, const char **errstrp, int base);
 *
 * DESCRIPTION
 * The strtonum() function converts the string in numstr to a long value. The strtonum()
 * function was designed to facilitate safe, robust programming and overcome the shortcomings
 * of the atoi(3) and strtol(3) family of interfaces.
 * The string may begin with an arbitrary amount of whitespace (as determined by isspace(3))
 * followed by a single optional '+' or '-' sign.
 *
 * The remainder of the string is converted to a long value according to the given base, which
 * must be a number between 2 and 36 inclusive or the special value 0 (see strtol(3)).
 *
 * The value obtained is then checked against the provided minval and maxval bounds. If errstrp
 * is non-null, strtonum() stores an error string in *errstrp indicating the failure.
 *
 * RETURN VALUES
 * The strtonum() function returns the result of the conversion, unless the value would exceed
 * the provided bounds or is invalid. On error, 0 is returned, errno is set, and errstrp will
 * point to an error message. *errstrp will be set to NULL on success; this fact can be used to
 * differentiate a successful return of 0 from an error.
 *
 * EXAMPLES
 * Using strtonum() correctly is meant to be simpler than the alternative functions.
 *
 * int iterations;
 * const char *errstr;
 *
 * iterations = strtonum(optarg, 1, 64, &errstr, 10);
 * if (errstr)
 *    errx(1, "number of iterations is %s: %s", errstr, optarg);
 *
 * The above example will guarantee that the value of iterations is between 1 and 64 (inclusive).
 *
 * ERRORS
 * [ERANGE]    The given string was out of range.
 * [EINVAL]    The given string did not consist solely of digit characters.
 * [EINVAL]    minval was larger than maxval.
 *
 * If an error occurs, errstrp will be set to one of the following strings:
 *
 * "too large"  The result was larger than the provided maximum value.
 * "too small"  The result was smaller than the provided minimum value.
 * "invalid"    The string did not consist solely of digit characters.
 *
 * SEE ALSO
 * atof(3), atoi(3), atol(3), atoll(3), sscanf(3), strtod(3), strtol(3), strtoul(3)
 *
 * STANDARDS
 * strtonum() is an OpenBSD extension. The existing alternatives, such as atoi(3) and strtol(3),
 * are either impossible or difficult to use safely.
 */

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "strtonum.h"

#define INVALID     1
#define TOOSMALL    2
#define TOOLARGE    3

long
strtonum_OBSD(const char * numstr, long minval, long maxval,
    const char **errstrp, int base)
{
    long l = 0;
    int error = 0;
    char * ep;
    struct errval {
        const char * errstr;
        int err;
    } ev[4] = {
        { NULL,     0 },
        { "invalid",    EINVAL },
        { "too small",  ERANGE },
        { "too large",  ERANGE },
    };

    ev[0].err = errno;
    errno = 0;

    if (minval > maxval) {
        error = INVALID;
    } else {
        l = strtol(numstr, &ep, base);

        if (numstr == ep || *ep != '\0') {
            error = INVALID;
        } else if ((l == LONG_MIN && errno == ERANGE) || l < minval) {
            error = TOOSMALL;
        } else if ((l == LONG_MAX && errno == ERANGE) || l > maxval) {
            error = TOOLARGE;
        }
    }

    if (errstrp != NULL) {
        *errstrp = ev[error].errstr;
    }

    errno = ev[error].err;

    if (error) {
        l = 0;
    }

    return (l);
}
