/*
 * File:   tools.c
 * Author: Scott Bennett
 */

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>

#include "bool.h"
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
 * Return the data in a specified byte number (o-3) of a source int.
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
 * Return 1 if source is negatrive, 0 otherwise.
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
 * Write 'length' zeroes to the buffer, effectively "clearing" it. If length is
 * zero, this function does nothing. Can clear a buffer of any size, up to
 * SIZE_MAX.
 *
 * Parameters:
 *     *buff    buffer
 *     length   length of the buffer
 */
void clearBuffer(char * buff, size_t length) {
    char * p;

    for (p = buff; length--; ) {
        *p++ = 0;
    }
}

/*
 * Reliably convert string value to an integer, between INT_MIN and INT_MAX.
 * Base must be a number between 2 and 36 inclusive or the special value 0.
 *
 * Parameters:
 *     *nptr   string representation of an int
 *     base    a base between 2 and 36 inclusive, or 0
 *
 * Returns the result of the conversion, or 0 on error
 */
int strtoint(const char * nptr, int base) {
    int num;
    num = (int) strtonum_OBSD(nptr, INT_MIN, INT_MAX, NULL, base);

    return num;
}

