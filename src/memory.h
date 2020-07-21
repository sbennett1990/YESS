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

#ifndef	MEMORY_H
#define	MEMORY_H

#include <stdint.h>

#include "bool.h"

#define MEMSIZE     1024	// 1024 words of memory
#define WORDSIZE       4	// Size of one word in bytes
#define HIGHBYTE    4095	// Highest byte address in memory
#define NOADDRESS      0	// Value for a "no address"

void clearMemory(void);

uint8_t getByte(int address, bool *memError);
void putByte(int address, uint8_t value, bool *memError);

unsigned int getWord(int address, bool * memError);
void putWord(int address, unsigned int value, bool * memError);

#endif	/* MEMORY_H */
