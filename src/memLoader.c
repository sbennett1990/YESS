/*
 * Copyright (c) 2020 Scott Bennett <scottb@fastmail.com>
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

#include <sys/types.h>

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memLoader.h"
#include "memory.h"
#include "logger.h"
#include "strtonum.h"
#include "tools.h"

#define LINE_LEN		16
#define MIN_LINE_LEN		14
#define MAX_LINE_LEN		32

#ifndef nitems
#define nitems(_a) (sizeof((_a)) / sizeof((_a)[0]))
#endif

struct memory_record {
	char		*line;		/* contents of 1 line from file */
	size_t		 linesize;	/* size of the line string */
	ssize_t		 linelen;	/* length of the line */
	int		 lineno;	/* line number from the file */
	short		 memaddress;	/* memory address (on word boundry) */
	short		 prevaddress;	/* previous line's memory address */
	unsigned int	 data;		/* data to store in memory */
	unsigned int	 prevdata;	/* previous line's data */
	int		 starline;	/* does line have a '*'? */
	int		 prevstarline;	/* did previous line have a '*'? */
};

static int validatememfilename(const char *filename);
static int validateline(struct memory_record *);
static bool hashexdigits(const char *line, ssize_t len, int start, int end);
static int readaddress(const char *line, int *error);
static unsigned int readdata(const char *line, int *error);
static unsigned int strtouint(const char *nptr, int base, int *error);
static int putrepeatdata(const struct memory_record * const record);

/*
 * Load a memory image from a file.
 */
bool
load_mem_image(const char *fileName)
{
	FILE *fp;

	if (validatememfilename(fileName) == -1) {
		return FALSE; /* EXIT */
	}

	/* open file as read-only */
	log_debug("opening file \'%s\'", fileName);
	fp = fopen(fileName, "r");
	if (fp == NULL) {
		log_warn("error opening the file");
		return FALSE; /* EXIT */
	}

	log_info("reading contents into memory");

	struct memory_record record = {
		.line = NULL,
		.linesize = 0,
		.lineno = 1,
		.prevaddress = -1,	/* starting address */
		.prevstarline = 0
	};

	/* attempt to load each line of the file into memory */
	while ((record.linelen = getline(&record.line, &record.linesize, fp)) != -1) {
		if (validateline(&record) == -1) {
			/* display the erroneous line */
			printf("Error on line %d:\n", record.lineno);

			for (int i = 0; i < record.linelen; i++) {
				printf("%c", record.line[i]);
			}
			printf("\n");
			goto error;
		}

		int error;
		record.data = readdata(record.line, &error);
		if (error) {
			log_info("error reading data on line %d",
			    record.lineno);
			goto error;
		}

		/*
		 * If previous line had a '*', then all addresses up to
		 * the current line's address will contain the previous
		 * line's data.
		 */
		if (record.prevstarline) {
			if (putrepeatdata(&record) == -1) {
				goto error;
			}
		}

		/* now store current line */
		bool memError;
		putWord(record.memaddress, record.data, &memError);
		if (memError) {
			log_info("error storing data at address %03x",
			    record.memaddress);
			goto error;
		}

		record.prevaddress = record.memaddress;
		record.prevdata = record.data;
		record.prevstarline = record.starline;
		record.lineno++;
	}

	free(record.line);
	log_debug("closing file");
	fclose(fp);
	return TRUE;

error:
	free(record.line);
	log_debug("closing file");
	fclose(fp);
	return FALSE;
}

/*
 * Validate that the given filename ends in ".mem"
 *
 * Parameters:
 *	*filename    filename to check
 *
 * Return >0 if file ends in ".mem"; -1 otherwise
 */
int
validatememfilename(const char *filename)
{
#ifdef AFL_TEST
	return 1;
#endif
	int len = (int)strnlen(filename, FILENAME_LEN);

	if (len < 4) {
		log_warn("filename too short");
		return -1;
	}

	if (filename[len - 1] == 'm'
	    && filename[len - 2] == 'e'
	    && filename[len - 3] == 'm'
	    && filename[len - 4] == '.') {
		log_debug("filename valid");
		return 1;
	}
	else {
		log_warn("filename not valid");
		return -1;
	}
}

/*
 * Determine if the line from the file is in the correct
 * format. Also validates that the memory address is
 * correct; sets the record's memaddress member as a
 * side effect.
 *
 * Parameters:
 *	*record       the line to check
 *
 * Return 1 if the line is correctly formatted.
 */
int
validateline(struct memory_record *record)
{
	if (record->linelen < MIN_LINE_LEN || record->linelen > LINE_LEN) {
		return -1;
	}

	/* column 3 should have a colon */
	if (record->line[3] != ':') {
		return -1;
	}

	/* column 4 should have a blank space */
	if (!isblank(record->line[4])) {
		return -1;
	}

	/* first 3 characters should be an address in hex */
	if (!hashexdigits(record->line, record->linelen, 0, 2)) {
		return -1;
	}

	/* characters 5 through 12 are one word of memory */
	if (!hashexdigits(record->line, record->linelen, 5, 12)) {
		return -1;
	}

	/* check if this line has a '*' */
	record->starline = 0;
	if (record->linelen > MIN_LINE_LEN) {
		/*
		 * if present, column 14 should have a '*', but a space
		 * is allowed
		 */
		if (record->line[14] != '*' && !isspace(record->line[14])) {
			return -1;
		}
		if (record->line[14] == '*') {
			record->starline = 1;
		}
	}

	/* validate the memory address is correct */
	int error;
	int memaddr = readaddress(record->line, &error);
	if (error) {
		log_info("error reading memory address on line %d",
		    record->lineno);
		return -1;
	}
	if (memaddr > HIGHBYTE) {
		log_info("address is too large: %d", memaddr);
		return -1;
	}
	record->memaddress = memaddr;

	/* memory image files begin at address 0 */
	if (record->prevaddress == -1) {
		if (record->memaddress != 0) {
			log_debug("memory image address must begin at 0");
			return -1;
		}
	}
	/*
	 * If the _previous_ line was a star line, memaddress should be
	 * at least prevaddress + 8; but memaddress should always be a
	 * multiple of WORDSIZE (currently 4).
	 */
	else if (record->prevstarline) {
		if (record->memaddress < record->prevaddress + 2 * WORDSIZE ||
		    record->memaddress % WORDSIZE) {
			log_debug("address is not correct: "
			    "curr: %03x, prev: %03x",
			    record->memaddress, record->prevaddress);
			return -1;
		}
	}
	else if (record->memaddress != record->prevaddress + WORDSIZE) {
		log_debug("address is not correct: curr: %03x, prev: %03x",
		    record->memaddress, record->prevaddress);
		return -1;
	}

	return 1;
}

/*
 * Check to see if the string contains only hex digits from start
 * index through end index, inclusive.
 *
 * Parameters:
 *     *line      one record to check
 *	len         length of the line string
 *     start      the starting index
 *     end        the ending index
 *
 * Return true if every char is a hex digit.
 */
bool
hashexdigits(const char *line, ssize_t len, int start, int end)
{
	if (start > end || start < 0 || end < 0) {
		return FALSE;
	}

	if (len <= end) {
		return FALSE;
	}

	int i;
	for (i = start; i <= end; i++) {
		if (!isxdigit(line[i])) {
			return FALSE;
		}
	}

	return TRUE;
}

/*
 *
 */
int
readaddress(const char *line, int *error)
{
	char hexaddr[4] = {
		line[0],
		line[1],
		line[2],
		'\0'
	};
	*error = 0;

	int memaddr = strtoint(hexaddr, 16);
	if (memaddr == -1) {
		log_debug("error converting string '%s' to an address",
		    hexaddr);
		*error = 1;
	}
	return memaddr;
}

/*
 *
 */
unsigned int
readdata(const char *line, int *error)
{
	char datastr[9] = {
		line[5],
		line[6],
		line[7],
		line[8],
		line[9],
		line[10],
		line[11],
		line[12],
		'\0'
	};
	*error = 0;

	unsigned int word = strtouint(datastr, 16, error);
	if (*error == 1) {
		log_debug("error converting string '%s' to data",
		    datastr);
		return 0;
	}

	return word;
}

/*
 * Reliably convert string value to an unsigned integer, between 0 and
 * UINT_MAX.
 * Base must be either 10 or 16.
 *
 * Parameters:
 *	*nptr   string representation of an int
 *	base    base 10 or base 16
 *
 * Returns the result of the conversion, or 0 on error.
 */
unsigned int
strtouint(const char *nptr, int base, int *error)
{
	unsigned int num;
	const char *errstr;
	*error = 0;

	num = (unsigned int)strtonum_OBSD(nptr, 0, UINT_MAX, &errstr, base);
	if (errstr != NULL) {
		log_debug("can't convert to uint: nptr is %s: %s\n", errstr,
		    nptr);
		*error = 1;
		return 0;
	}

	return num;
}

/*
 *
 */
int
putrepeatdata(const struct memory_record * const record)
{
	bool memError;
	short addr = record->prevaddress + WORDSIZE;
	for (; addr < record->memaddress; addr += WORDSIZE) {
		putWord(addr, record->prevdata, &memError);
		if (memError) {
			log_info("error storing repeat data at address %03x",
			    addr);
			return -1;
		}
	}

	return 1;
}
