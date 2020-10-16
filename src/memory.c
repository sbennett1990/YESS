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

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "tools.h"

#include "logger.h"

/* System Memory */
static unsigned int *memory;

static unsigned int fetch(int address, bool * memError);
static void store(int address, unsigned int value, bool * memError);

/*
 * Retrieve a word from memory. If there is an error,
 * *memError is set to true.
 *
 * Parameters:
 *	address     the word address [0..1023]
 *	*memError   indicates memory read error
 *
 * Return the contents of memory at address, or 0 on error.
 */
unsigned int
fetch(int address, bool * memError)
{
    if (address < 0 || address >= MEMSIZE) {
        *memError = TRUE;
        return 0;
    }

    *memError = FALSE;
    return memory[address];
}

/*
 * Write a value (1 word) to memory. If there is an error,
 * *memError is set to true. If address isn't a valid memory
 * address, then memory isn't modified.
 *
 * Parameters:
 *	address     the word address [0..1023]
 *	value       the value to store in memory at address
 *	*memError   indicates memory write error
 */
void
store(int address, unsigned int value, bool * memError)
{
    if (address < 0 || address >= MEMSIZE) {
        *memError = TRUE;
        return;
    }

    *memError = FALSE;
    memory[address] = value;
}

/*
 * Read a byte of memory using it's byte address. If there is
 * an error, *memError is set to true.
 *
 * Parameters:
 *	byteAddress    address of a byte of memory [0..4095]
 *	*memError      memory error indicator
 *
 * Return the contents of memory at the byte address, or 0 on error.
 */
uint8_t
getByte(int byteAddress, bool *memError)
{
	if (byteAddress < 0 || byteAddress > HIGHBYTE) {
		log_debug("can't get byte: invalid memory address %08x",
		    byteAddress);
		*memError = TRUE;
		return 0;
	}

	*memError = FALSE;

	/*
	 * Retrieve the word containing the specified byte address using
	 * integer division
	 */
	unsigned int word = fetch((byteAddress / WORDSIZE), memError);
	if (*memError) {
		log_debug("can't get byte: error fetching containing word");
		return 0;
	}

	return getByteNumber((byteAddress % WORDSIZE), word);
}

/*
 * Store a 1-byte value in memory. If there is an error,
 * *memError is set to true. If address isn't a valid memory
 * address, then memory isn't modified.
 *
 * Parameters:
 *	byteAddress    address of a byte of memory [0..4095]
 *	value          the 8-bit value to store in memory
 *	*memError      memory error indicator
 */
void
putByte(int byteAddress, uint8_t value, bool *memError)
{
	if (byteAddress < 0 || byteAddress > HIGHBYTE) {
		log_debug("can't put byte: invalid memory address %08x",
		    byteAddress);
		*memError = TRUE;
		return;
	}

	*memError = FALSE;

	/*
	 * Retrieve the word containing the specified byte address using
	 * integer division
	 */
	unsigned int word = fetch((byteAddress / WORDSIZE), memError);
	if (*memError) {
		log_debug("can't put byte: error fetching containing word");
		return;
	}

	// Modify the byte address of the word, store in newWord
	unsigned int newWord = putByteNumber((byteAddress % WORDSIZE), value,
	    word);

	store((byteAddress / WORDSIZE), newWord, memError);
}

/*
 * Retrieve a word from memory at the given byte address. Word accesses
 * must be aligned on a 4 byte boundary. If there is an error, *memError
 * is set to true.
 *
 * Parameters:
 *	byteAddress   a byte address of memory [0..4095] that is a
 *                multiple of 4
 *	*memError     indicates memory read error
 *
 * Return the contents of memory at byte address, or 0 on error.
 */
unsigned int
getWord(int byteAddress, bool * memError)
{
	// TODO: test this function!
	if (byteAddress < 0 || byteAddress > HIGHBYTE) {
		log_debug("can't get word: invalid memory address %08x",
		    byteAddress);
		*memError = TRUE;
		return 0;
	}

	if (byteAddress == 0) {
		return fetch(0, memError);
	}

	/* ensure byteAddress is a multiple of WORDSIZE */
	if (byteAddress % WORDSIZE) {
		log_debug("can't get word: %03x is not a multiple of 4",
		    byteAddress);
		*memError = TRUE;
		return 0;
	}

	*memError = FALSE;
	// How does this work if byteAddress is 1 or 0?
	return fetch((byteAddress / WORDSIZE), memError);
}

/*
 * Write a value (1 word) to memory at the given byte address. Word accesses
 * must be aligned on a 4 byte boundary. If there is an error, *memError is
 * set to true. If address isn't a valid memory address, then memory isn't
 * modified.
 *
 * Parameters:
 *	byteAddress   a byte address of memory [0..4095] that is a
 *                multiple of 4
 *	value         the word to store in memory at address
 *	*memError     indicates memory write error
 */
void
putWord(int byteAddress, unsigned int value, bool * memError)
{
	// TODO: test this function!
	if (byteAddress < 0 || byteAddress > HIGHBYTE) {
		log_debug("can't put word: invalid memory address %08x",
		    byteAddress);
		*memError = TRUE;
		return;
	}

	/* ensure byteAddress is a multiple of WORDSIZE */
	if (byteAddress % WORDSIZE) {
		log_debug("can't put word: %03x is not a multiple of 4",
		    byteAddress);
		*memError = TRUE;
		return;
	}

	*memError = FALSE; // XXX: this assignment is probably unnecessary
	store((byteAddress / WORDSIZE), value, memError);
}

/*
 * Initialize yess system memory to MEMSIZE, setting all memory
 * locations to 0.
 */
void
initMemory(void)
{
	if ((memory = calloc(MEMSIZE, sizeof(int))) == NULL) {
		err(1, NULL);
	}
}

/*
 * Free the memory buffer. This should be called before main exits.
 */
void
destroyMemory(void)
{
	free(memory);
}

/*
 * Set all memory locations to 0.
 */
void
clearMemory(void)
{
	memset(memory, 0, sizeof(memory));
}
