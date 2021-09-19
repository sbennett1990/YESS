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

#include <sys/types.h>

#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "loader.h"
#include "logger.h"
#include "tools.h"

#define RECORDLEN	25
#define MAXLEN		80

#define HEX		16

static bool validline(const char *line, size_t len, int prev_addr);
static bool iscommentrecord(const char *line, size_t len);
static bool isdatarecord(const char *line, size_t len);
static bool hasaddress(const char *line, size_t len);
static bool hasspaces(const char *line, size_t len, unsigned int start,
    unsigned int end);
static bool hashexdigits(const char *line, size_t len, unsigned int start,
    unsigned int end);
static short hasdata(const char *line, size_t len);
static bool validateaddress(const char *line, size_t len, int prev_addr);
static bool validatedata(const char *record);
static int grabAddress(const char *line, int *error);
static uint8_t grabDataByte(const char *record, short byteNum, bool *error);

/*
 * Loads machine source code (instructions and data) into yess
 * memory. Also performs error checking on the source file.
 *
 * Parameters:
 *  *fileName	name of the file to load
 *
 * Return true if load was successful; false if error occurred
 */
bool
load(const char * fileName)
{
	FILE *fp;
	bool memError;

	if (validatefilename(fileName) == -1) {
		return FALSE; /* EXIT */
	}

	/* open file as read-only */
	log_debug("opening file \'%s\'", fileName);
	fp = fopen(fileName, "r");
	if (fp == NULL) {
		log_warn("error opening the file");
		return FALSE; /* EXIT */
	}

	log_info("reading program into memory");

	/* initial value since no address has been modified yet */
	int prevaddr = -1;
	int lineno = 1;
	int byteAddress;        // memory address of one byte [0..HIGHBYTE]

	char *record = NULL;
	size_t recordsize = 0;
	ssize_t linelen;
	char buf[RECORDLEN];
	/* attempt to load each line of the file into memory */
	while ((linelen = getline(&record, &recordsize, fp)) != -1) {
		strlcpy(buf, record, sizeof(buf));

		// Error checking...
		// 1: check line format (ie, don't check address correctness yet)
		// 2: if address line, then check that the address is > prevaddr
		if (!validline(buf, sizeof(buf), prevaddr)) {
			/* Display the erroneous line */
			printf("Error on line %d\n", lineno);
			printf("%s\n", record);
			goto error;
		}

		if (hasaddress(buf, sizeof(buf))) {
			int error;
			byteAddress = grabAddress(buf, &error);
			if (error) {
				log_info("error extracting address");
				goto error;
			}

			short numBytes;
			if ((numBytes = hasdata(buf, sizeof(buf))) != 0) {
				for (short byteNumber = 1; byteNumber <= numBytes; byteNumber++) {
					uint8_t data; /* byte to store in mem */
					data = grabDataByte(buf, byteNumber, &memError);
					if (memError) {
						goto error;
					}

					putByte(byteAddress, data, &memError);
					if (memError) {
						goto error;
					}

					byteAddress++;
				}

				prevaddr = byteAddress - 1;
			}
		}

		lineno++;
	}

	free(record);
	if (ferror(fp)) {
		fatal("getline");  // TODO: better diagnostic?
	}
	log_debug("closing file");
	fclose(fp);
	return TRUE;

error:
	free(record);
	log_debug("closing file");
	fclose(fp);
	return FALSE;
}

/*
 * Determine if the record (one line from the file) is in the correct
 * format (syntactically and semantically correct).
 *
 * Parameters:
 *     *line       the line to check
 *     prev_addr   the previously written-to memory address
 *
 * Return true if the line is correctly formatted; false otherwise
 */
bool
validline(const char *line, size_t len, int prev_addr)
{
	assert(line != NULL);

	if (len < RECORDLEN) {
		return FALSE;
	}

	// the pipe character is supposed to be on every line
	if (line[22] != '|') {
		return FALSE;
	}
	// columns 0, 1, 8, and 21 should always have a blank space
	if (!(isblank(line[0]) && isblank(line[1]) && isblank(line[8])
	    && isblank(line[21]))) {
		return FALSE;
	}

	if (iscommentrecord(line, len)) {
		return TRUE;
	}

	/* this must be a data line */

	if (!isdatarecord(line, len)) {
		return FALSE;
	}

	// check for address and data correctness
	bool ret = FALSE;
	if (validateaddress(line, len, prev_addr)) {
		ret = TRUE;

		if (hasdata(line, len)) {
			ret = validatedata(line);
		}
	}
	return ret;
}

/*
 * Determine if the line is a comment line.
 */
bool
iscommentrecord(const char *line, size_t len)
{
	assert(line != NULL);
	assert(len > 21);

	if (hasspaces(line, len, 0, 21)) {
		return TRUE;
	}

	return FALSE;
}

/*
 * Determine if the line is a data line (i.e. has an address and data,
 * if applicable).
 */
bool
isdatarecord(const char *line, size_t len)
{
	assert(line != NULL);
	assert(len > 21);

	if (!(isblank(line[0]) && isblank(line[1]) && isblank(line[8])
	    && isblank(line[21]))) {
		return FALSE;
	}

	if (line[2] == '0'
	    && line[3] == 'x'
	    && line[7] == ':') {
		return TRUE;
	}

	return FALSE;
}

/*
 * Determine if the line contains an address. No error
 * checking is done on the address, but the syntax of
 * the address is checked.
 *
 * Parameters:
 *     *line    the line to check
 *
 * Return true if the record has an address; false otherwise
 */
bool
hasaddress(const char *line, size_t len)
{
	assert(line != NULL);
	assert(len > 7);

	if (line[2] == '0'
	    && line[3] == 'x'
	    && line[7] == ':') {
		if (isxdigit(line[4]) && isxdigit(line[5]) && isxdigit(line[6])) {
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * Check to see if there are spaces from start index
 * through end index.
 *
 * Parameters:
 *	*line     the to check
 *	start     starting index
 *	end       ending index
 *
 * Return true if record contains spaces at indices;
 * false otherwise
 */
bool
hasspaces(const char *line, size_t len, unsigned int start, unsigned int end)
{
	assert(line != NULL);
	assert(start <= end);

	if (len <= end) {
		return FALSE;
	}

	for (unsigned int i = start; i <= end; i++) {
		if (line[i] != ' ') {
			return FALSE;
		}
	}

	return TRUE;
}

/*
 * Check to see if the record contains hex from start index
 * through end index.
 *
 * Parameters:
 *     *line      one record to check
 *     start      the starting index
 *     end        the ending index
 *
 * Return true if every digit is a hex digit; false otherwise
 */
bool
hashexdigits(const char *line, size_t len, unsigned int start, unsigned int end)
{
	assert(line != NULL);
	assert(start <= end);

	if (len <= end) {
		return FALSE;
	}

	for (unsigned int i = start; i <= end; i++) {
		if (!(isxdigit(line[i]))) {
			return FALSE;
		}
	}

	return TRUE;
}

/*
 * Get the address out of the data record. This
 * function performs no error checking and assumes
 * the record is in fact a data record.
 *
 * Parameters:
 *	*record     a record with an address
 *	*error      indicates if an error occured
 *
 * Return the address in base 10
 */
int
grabAddress(const char *record, int *error)
{
	assert(record != NULL);

	char hex_addr[8] = {
		record[0],
		record[1],
		record[2],
		record[3],
		record[4],
		record[5],
		record[6],
		'\0'
	};
	*error = 0;

	// TODO: check for the error value -1
	int addr = strtoint(hex_addr, HEX);
	if (addr == -1) {
		log_debug("error converting string '%s' to an address",
		    hex_addr);
		*error = 1;
	}
	return addr;
}

/*
 * Validate that the address in the data record is formatted
 * correctly. Also check if the address in the data
 * record is greater than the previous address of memory
 * where data was stored. If prev_addr is -1, then there
 * was no previous address. This function assumes that
 * the record being checked does contain an address.
 *
 * Parameters:
 *	*line       a line with an address
 *	prev_addr   the previously written-to memory address
 *
 * Return true if the address is correct; false otherwise
 */
bool
validateaddress(const char *line, size_t len, int prev_addr)
{
	assert(line != NULL);

	if (!hashexdigits(line, len, 4, 6)) {
		// address not formatted correctly
		return FALSE;
	}

	if (prev_addr == -1) {
		return TRUE;
	}

	// current must be > prev_addr
	int error;
	int current_addr = grabAddress(line, &error);

	if (error || current_addr < prev_addr) {
		return FALSE;
	}

	return TRUE;
}

/*
 * Determine if the line contains data. No error
 * checking is performed. Data should be stored in
 * columns 9 through 20 as hex, with 0 to 6 bytes.
 *
 * Parameters:
 *     *line    the line to check
 *
 * Return the number of data bytes the record contains (0 to 6)
 */
short
hasdata(const char *line, size_t len)
{
	assert(line != NULL);
	assert(len > 21);

	/*
	 * since little error checking is performed, only examine
	 * the first index where data should be
	 */
	if (!isxdigit(line[9])) {
		return 0;
	}

	short start = 9;
	short end = 10;
	short num = 0;

	while (hashexdigits(line, len, start, end)) {
		start += 2;
		end += 2;
		num += 1;
	}

	return num;
}

/*
 * Validate that the data in the record is in the correct
 * format. Do not check for a hex digit in column
 * 21 since there is supposed to be a space character.
 * If there is a hex digit in column 21 the error
 * will be caught elsewhere. This function assumes
 * the record is supposed to contain data.
 *
 * Parameters:
 *     *record    one record to check
 *
 * Return true if the data is correctly formatted;
 * false otherwise
 */
bool
validatedata(const char *record)
{
	assert(record != NULL);

    int len = strnlen(record, RECORDLEN);

    if (len < 21) {
        return FALSE;
    }

    // data must be in at least columns 9 & 10
    if (!hashexdigits(record, len, 9, 10)) {
        return FALSE;
    }

    int i;

    for (i = 11; i <= 20; i += 2) {
        if (isxdigit(record[i])) {
            // next column should be a hex digit
            if (!isxdigit(record[i + 1])) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/*
 * Returns one byte of data at the index in the record.
 *
 * Parameters:
 *	*record    the data record to search
 *	byteNum    the byte number of the data to grab out of the record
 *
 * Return one byte of data from the record
 */
uint8_t
grabDataByte(const char *record, short byteNum, bool *error)
{
	assert(record != NULL);

	*error = FALSE;
/*
	if (byteNum < 1 || byteNum > 4) {
		log_debug("byteNum is invalid");
		*error = TRUE;
	}
*/
/* Calculate the start of the data byte */
#define WHICH_BYTE_IDX(n)	(((n) * 2) + 7)

	int index = WHICH_BYTE_IDX(byteNum);
	char bytestr[3] = {
		record[index],
		record[index + 1],
		'\0'
	};

	int result = strtoint(bytestr, HEX);
	if (result == -1) {
		log_debug("error converting string '%s' to data",
		    bytestr);
		*error = TRUE;
		return 0;
	}

	return (uint8_t)result;
}
