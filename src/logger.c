/*  $OpenBSD: log.c,v 1.13 2015/12/19 17:55:29 reyk Exp $   */

/*
 * Copyright (c) 2003, 2004 Henning Brauer <henning@openbsd.org>
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

int          debug;
int          verbose;
const char * log_procname;

static void log_procinit(const char *);
static void vlog(const char *, va_list);
static void vfatal(const char * emsg, va_list ap);

void log_init(int, int);
void log_verbose(int);
void log_warn(const char *, ...);
void log_warnx(const char *, ...);
void log_info(const char *, ...);
void log_debug(const char *, ...);
void logit(const char *, ...);
void fatal(const char *, ...);
void fatalx(const char *, ...);

void
log_init(int n_debug, int facility) {
    debug = n_debug;
    verbose = n_debug;
    log_procinit("yess");
}

void
log_procinit(const char * procname) {
    if (procname != NULL) {
        log_procname = procname;
    }
}

void
log_verbose(int v) {
    verbose = v;
}

void
logit(const char * fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vlog(fmt, ap);
    va_end(ap);
}

void
vlog(const char * fmt, va_list ap) {
    char  *  nfmt;

    if (debug) {
        /* best effort in out of mem situations */
        if (asprintf(&nfmt, "%s\n", fmt) == -1) {
            vfprintf(stderr, fmt, ap);
            fprintf(stderr, "\n");
        } else {
            vfprintf(stderr, nfmt, ap);
            free(nfmt);
        }

        fflush(stderr);
    }
}


void
log_warn(const char * emsg, ...) {
    char  *  nfmt;
    va_list  ap;

    /* best effort to even work in out of memory situations */
    if (emsg == NULL) {
        logit("%s", strerror(errno));
    } else {
        va_start(ap, emsg);

        if (asprintf(&nfmt, "%s: %s", emsg, strerror(errno)) == -1) {
            /* we tried it... */
            vlog(emsg, ap);
            logit("%s", strerror(errno));
        } else {
            vlog(nfmt, ap);
            free(nfmt);
        }

        va_end(ap);
    }
}

void
log_warnx(const char * emsg, ...) {
    va_list  ap;

    va_start(ap, emsg);
    vlog(emsg, ap);
    va_end(ap);
}

void
log_info(const char * emsg, ...) {
    va_list  ap;

    va_start(ap, emsg);
    vlog(emsg, ap);
    va_end(ap);
}

void
log_debug(const char * emsg, ...) {
    va_list  ap;

    if (verbose > 1) {
        va_start(ap, emsg);
        vlog(emsg, ap);
        va_end(ap);
    }
}

void
vfatal(const char * emsg, va_list ap) {
    static char s[BUFSIZ];
    const char * sep;

    if (emsg != NULL) {
        (void)vsnprintf(s, sizeof(s), emsg, ap);
        sep = ": ";
    } else {
        s[0] = '\0';
        sep = "";
    }

    if (errno)
        logit("%s: %s%s%s",
              log_procname, s, sep, strerror(errno));
    else {
        logit("%s%s%s", log_procname, sep, s);
    }
}

/*
 * Used under fatal conditions. Application will exit.
 */
void
fatal(const char * emsg, ...) {
    va_list ap;

    va_start(ap, emsg);
    vfatal(emsg, ap);
    va_end(ap);
    exit(1);
}

/*
 * Used under fatal conditions. Application will exit. errno will be set to 0.
 */
void
fatalx(const char * emsg, ...) {
    va_list ap;

    errno = 0;
    va_start(ap, emsg);
    vfatal(emsg, ap);
    va_end(ap);
    exit(1);
}
