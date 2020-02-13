/*
 * memory.c
 */

#include "tools.h"
#include "memory.h"

static unsigned int memory[MEMSIZE]; // XXX: should MEMSIZE be 1023?

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
static unsigned int
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
static void
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
 * an error, *memError is set to true, otherwise it's false.
 *
 * Parameters:
 *  byteAddress     a byte address of memory [0..4095]
 *  *memError       pointer to the memory error indicator
 *
 * Return the contents of memory at the byte address, or 0 for error
 */
unsigned char getByte(int byteAddress, bool * memError) {
    if (byteAddress < 0 || byteAddress > HIGHBYTE) {
        *memError = TRUE;
        return 0;
    }

    *memError = FALSE;

    /*
     * Retrieve the word containing the specified byte address using integer
     * division
     */
    unsigned int word = fetch((byteAddress / WORDSIZE), memError);

    return getByteNumber((byteAddress % WORDSIZE), word);
}

/*
 * Store a value (1 byte) in memory. If there is an error,
 * *memError is set to true, otherwise it's false. If address
 * isn't a valid memory address, then memory isn't modified.
 *
 * Parameters:
 *  byteAddress     a byte address of memory [0..4095]
 *  value           the value to store in memory at the address
 *  *memError       pointer to the memory error indicator
 */
void putByte(int byteAddress, unsigned char value, bool * memError) {
    if (byteAddress < 0 || byteAddress > HIGHBYTE) {
        *memError = TRUE;
        return;
    }

    *memError = FALSE;

    /*
     * Retrieve the word containing the specified byte address using integer
     * division
     */
    unsigned int word = fetch((byteAddress / WORDSIZE), memError);

    // Modify the byte address of the word, store in newWord
    unsigned int newWord = putByteNumber((byteAddress % WORDSIZE), value, word);

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
		*memError = TRUE;
		return 0;
	}

	if (byteAddress == 0) {
		return fetch(0, memError);
	}

    /* ensure byteAddress is a multiple of WORDSIZE */
    if (byteAddress % WORDSIZE) {
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
		*memError = TRUE;
		return;
	}

	/* ensure byteAddress is a multiple of WORDSIZE */
	if (byteAddress % WORDSIZE) {
		*memError = TRUE;
		return;
	}

	*memError = FALSE; // XXX: this assignment is probably unnecessary
	store((byteAddress / WORDSIZE), value, memError);
}

/*
 * Set all memory locations to 0.
 */
void
clearMemory(void)
{
	clearBuffer((char *) &memory, MEMSIZE);
}
