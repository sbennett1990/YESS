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

/* pledge(2) the program on OpenBSD */
#ifdef __OpenBSD__
#include <sys/utsname.h>
#endif

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include <err.h>
#include <unistd.h>

#include "logger.h"
#include "strtonum.h"
#include "tools.h"

/*
 * Returns the bits in the range low to high from source.
 *
 * Parameters:
 *      low     rightmost bit
 *      high    leftmost bit
 *      source  integer to extract bit range from
 *
 * Return 0 for error, else the unsigned integer extracted from source.
 */
unsigned int getBits(int low, int high, unsigned int source) {
    if (low < 0 || high > 31 || low > high) {
        return 0;
    }

    int leftAmount = 31 - high;         // number of places to left shift source
    int rightAmount = low + leftAmount; // number of places to right shift source

    // perform two shifts to extract the bits [low..high]
    unsigned int answer  = source << leftAmount;
    answer = answer >> rightAmount;

    return answer;
}

/*
 * Change the bits in the range [low..high] in source to 1's.
 *
 * Parameters:
 *      low     rightmost bit
 *      high    leftmost bit
 *      source  integer to set the bits in
 *
 * Return source unchanged for error, else the unsigned integer
 * with the appropriate range of bits set to 1's.
 */
unsigned int setBits(int low, int high, unsigned int source) {
    if (low < 0 || high > INTHIGHBIT || low > high) {
        return source;
    }

    int leftAmount = INTHIGHBIT - high; // number of places to left shift source
    int rightAmount = low + leftAmount; // number of places to right shift source

    // setter number with appropriate bits set to 1's
    unsigned int setter = 0xffffffff;

    // shift the setter number so that only the [low..high] bits are 1's
    setter = setter << leftAmount;
    setter = setter >> rightAmount;
    setter = setter << low;

    // OR source and setter so bit range is 1's
    source = source | setter;

    return source;
}

/*
 * Change the bits in the range [low..high] in source to 0's.
 *
 * Parameters:
 *      low     rightmost bit
 *      high    leftmost bit
 *      source  integer to set the bits in
 *
 * Return source unchanged for error, else the unsigned integer
 * with the appropriate range of bits set to 0's.
 */
unsigned int clearBits(int low, int high, unsigned int source) {
    if (low < 0 || high > INTHIGHBIT || low > high) {
        return source;
    }

    int leftAmount = INTHIGHBIT - high; // number of places to left shift source
    int rightAmount = low + leftAmount; // number of places to right shift source

    // clearing number with appropriate bits set to 0's
    unsigned int clear = 0xffffffff;

    // shift the clearing number so that only the [low..high] bits are 0's
    clear = clear << leftAmount;
    clear = clear >> rightAmount;
    clear = clear << low;
    clear = ~clear;

    source = source & clear;  // AND source and clear so bit range is 0's

    return source;
}

/*
 * Set the value of one bit in source to either 0 or 1.
 *
 * Parameters:
 *      bitNumber   the bit to set, [0..31]
 *      bitValue    either 0 or 1
 *      source      integer being changed
 *
 * Return source unchanged for error, else the unsigned integer
 * with the appropriate bit set.
 */
unsigned int assignOneBit(int bitNumber, int bitValue, unsigned int source) {
    if (bitNumber < 0 || bitNumber > INTHIGHBIT ||
        bitValue < 0 || bitValue > 1) {
        return source;
    }

    unsigned int setter = 1;
    setter = setter << bitNumber;

    if (bitValue) {
        source = source | setter;
    } else {
        setter = ~setter;
        source = source & setter;
    }

    return source;
}

/*
 * Return the data in a specified byte number (0-3) of a source int.
 *
 * Parameters:
 *      byteNo  the byte of data to grab
 *      source  the source number
 *
 * Returns the byte of data from source, or 0 if the byte number is
 * invalid.
 */
unsigned char getByteNumber(int byteNo, unsigned int source) {
    if (byteNo < 0 || byteNo > 3) {
        return 0;
    }

    // create the bit range for the specified byte
    int low = BYTESIZE * byteNo;
    int high = low + (BYTESIZE - 1);

    // extract the bits from the specified byte and put them into a char
    char character = (char) getBits(low, high, source);

    return character;
}

/*
 * Replace a byte of data in the source number with a specified byte
 * of new data.
 *
 * Parameters:
 *      byteNo      byte number (0-3) in source
 *      byteValue   new byte of data
 *      source      the number to replace the byte
 *
 * Returns the source with the new byte of data, or if the byte
 * number is invalid returns source unchanged.
 */
unsigned int putByteNumber(int byteNo, unsigned char byteValue,
                           unsigned int source) {
    if (byteNo < 0 || byteNo > 3) {
        return source;
    }

    // create the bit range for the specified byte
    int low = BYTESIZE * byteNo;
    int high = low + (BYTESIZE - 1);

    // zero out the byte to be changed
    source = clearBits(low, high, source);
    // set the byte in source to byteValue
    unsigned int newByteValue = byteValue << low;
    source = source | newByteValue;

    return source;
}

/*
 * Take four bytes of data and build one unsigned int from these
 * bytes.
 *
 * Parameters:
 *      byte0   the 0th byte of data
 *      byte1   the 1st byte of data
 *      byte2   the 2nd byte of data
 *      byte3   the 3rd byte of data
 *
 * Returns the word built from the four bytes of data.
 */
unsigned int buildWord(unsigned char byte0, unsigned char byte1,
                       unsigned char byte2, unsigned char byte3) {
    unsigned int num = byte3;

    // shift the num over one byte and add the next byte; repeat
    num = num << BYTESIZE;
    num += byte2;

    num = num << BYTESIZE;
    num += byte1;

    num = num << BYTESIZE;
    num += byte0;

    return num;
}

/*
 * Test to see if an integer is negative.
 *
 * Parameters:
 *      source  integer to test
 *
 * Return 1 if source is negative, 0 otherwise.
 */
bool isNegative(unsigned int source) {
    source = source >> INTHIGHBIT;
    return source;
}

/*
 * Put the character binary representation of the integer
 * source into the array bits[].
 *
 * Parameters:
 *      source  an unsigned integer
 *      bits    array to put the binary of source into
 */
void expandBits(unsigned int source, char bits[36]) {
    bits[35] = 0;

    int i;

    for (i = 34; i >= 0; i--) {
        // put in the spaces to break up bytes
        if (i == 26 || i == 17 || i == 8) {
            bits[i] = 32;      // ascii " "
        } else {
            if (source % 2) {  // rightmost 1
                bits[i] = 49;    // ascii "1"
            } else {           // rightmost 0
                bits[i] = 48;    // ascii "0"
            }

            source = source >> 1;
        }
    }
}

/*
 * Reliably convert string value to an integer, between INT_MIN and INT_MAX.
 * Base must be either 10 or 16.
 *
 * Parameters:
 *	*nptr   string representation of an int
 *	base    base 10 or base 16
 *
 * Returns the result of the conversion, or -1 on error
 */
int
strtoint(const char *nptr, int base)
{
	assert(base == 10 || base == 16);

	int num;
	const char *errstr;

	num = (int)strtonum_OBSD(nptr, INT_MIN, INT_MAX, &errstr, base);
	if (errstr != NULL) {
		log_debug("can't convert to int: nptr is %s: %s\n", errstr,
		    nptr);
		return -1;
	}

	return num;
}

/*
 * Validate that the given string ends in ".yo".
 *
 * Parameters:
 *     *filename    file name to check
 *
 * Return >0 if file ends in ".yo"; -1 otherwise
 */
int
validatefilename(const char * filename)
{
	assert(filename != NULL);

    int len = (int) strnlen(filename, FILENAME_LEN);

    if (len < 3) {
        log_warn("filename too short");
        return -1;
    }

    if (filename[len - 1] == 'o'
        && filename[len - 2] == 'y'
        && filename[len - 3] == '.') {
        log_debug("filename valid");
        return 1;
    }
    else {
        log_warn("filename not valid");
        return -1;
    }
}

/*
 * Put all the complex system testing code in one place. Functions can pass a
 * list of promises. Be careful to only reduce privileges!
 */
static void
pledge_wrapper(const char * promises)
{
#ifdef __OpenBSD__
    // pledge(2) only works on 5.9 or higher
    struct utsname name;

    if (uname(&name) != -1 && strncmp(name.release, "5.8", 3) > 0) {
        if (pledge(promises, NULL) == -1) {
            err(1, "pledge");
        }

        log_info("pledge(2)'d with %s", promises);
    }
    else {
        log_info("not running OpenBSD >5.9, no pledge");
    }
#else
    log_info("not running OpenBSD, no pledge");
#endif
}

/*
 * If running on OpenBSD 5.9 or higher, reduce privileges to "stdio" and "rpath"
 * because a file has to be opened. If any other OS, do nothing.
 */
void
initialpledge(void)
{
	pledge_wrapper("stdio rpath");
}

/*
 * If running on OpenBSD 5.9 or higher, reduce privileges to "stdio" after
 * opening the file. If any other OS, do nothing.
 */
void
reduceprivileges(void)
{
	pledge_wrapper("stdio");
}
