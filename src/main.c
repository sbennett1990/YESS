/*
 * Copyright (c) 2014, 2016, 2020 Scott Bennett <scottb@fastmail.com>
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
#include <unistd.h>

#include "bool.h"
#include "tools.h"
#include "logger.h"
#include "memory.h"
#include "dump.h"
#include "loader.h"
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

int
main(int argc, char **argv)
{
	int ch;
	int dflag = 0;	/* debug */
	int fflag = 0;	/* was file given? */
	int vflag = 0;	/* verbose */
	int verbosity = 0;
	const char *sourcefile;

	while ((ch = getopt(argc, argv, "df:uv")) != -1) {
		switch (ch) {
		case 'd':
			dflag = 1;
			break;
		case 'f':
			sourcefile = optarg;
			fflag = 1;
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

	if (argc > 0 || sourcefile == NULL || !fflag) {
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
	log_debug("initializing yess...");
	initialpledge();

	/* initialize the 'processor' */
	setupyess();

	/* load 'program' file into 'memory'; terminate if there is a problem */
	if (!load(sourcefile)) {
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

	while (stop != -1) {
		stop = writebackStage(&forward);
		memoryStage(&forward);
		executeStage(&forward);
		decodeStage(&forward);
		fetchStage(&forward);
		clockCount++;	/* each loop iteration is 1 clock cycle */
	}

	/* XXX: need to not dump if error encountered */
#if 0
	if (dflag) {
		dumpProgramRegisters();
		dumpProcessorRegisters();
		dumpMemory();
	}
#endif
	printf("\nTotal clock cycles = %d\n", clockCount);
	return 0;
}
