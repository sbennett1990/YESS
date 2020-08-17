/*
 * Copyright (c) 2014 Scott Bennett <scottb@fastmail.com>
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

#ifndef TOOLS_H
#define TOOLS_H

#include <stddef.h>

#include "bool.h"

#define BYTESIZE	8   /* Size of one byte in bits */
#define INTHIGHBIT	31  /* Highest bit for type int */

#define FILENAME_LEN	50

unsigned int getBits(int low, int high, unsigned int source);
unsigned int setBits(int low, int high, unsigned int source);
unsigned int clearBits(int low, int high, unsigned int source);
unsigned int assignOneBit(int bitNumber, int bitValue, unsigned int source);
unsigned char getByteNumber(int byteNo, unsigned int source);
unsigned int putByteNumber(int byteNo, unsigned char byteValue,
    unsigned int source);
unsigned int buildWord(unsigned char byte0, unsigned char byte1,
    unsigned char byte2, unsigned char byte3);
bool isNegative(unsigned int source);
void expandBits(unsigned int source, char bits[36]);
void clearBuffer(char * buff, size_t length);
int strtoint(const char *nptr, int base);
int validatefilename(const char * filename);
void initialpledge(void);
void reduceprivileges(void);

#endif /* TOOLS_H */
