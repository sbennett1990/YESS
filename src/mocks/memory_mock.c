/*
 * memory_mock.c
 */

#include "../bool.h"
#include "../tools.h"
#include "memory_mock.h"

static unsigned int memory[MEMSIZE];

// Prototypes for "private" functions
static unsigned int fetch(int address, bool * memError);
static void store(int address, unsigned int value, bool * memError);

/*
 * Read and return a word of memory. If there is an error,
 * *memError is set to true, otherwise it's false.
 *
 * Parameters:
 *  address     the memory address [0..1023]
 *  *memError   pointer to the memory error indicator
 *
 * Return the contents of the memory address, or 0 on error.
 *
 * NOTE: This function can not be accessed outside
 * of memory.c
 */
unsigned int fetch(int address, bool * memError) {
    if (address < 0 || address >= MEMSIZE) {
        *memError = TRUE;
        return 0;
    }

    *memError = FALSE;
    return memory[address];
}

/*
 * Store a value (1 word) in memory. If there is an error,
 * *memError is set to true, otherwise it's false. If address
 * isn't a valid memory address, then memory isn't modified.
 *
 * Parameters:
 *  address     the memory address [0..1023]
 *  value       the value to store in memory at the address
 *  *memError   pointer to the memory error indicator
 *
 * NOTE: This function can not be accessed outside
 * of memory.c
 */
void store(int address, unsigned int value, bool * memError) {
    if (address < 0 || address >= MEMSIZE) {
        *memError = TRUE;
    } else {
        *memError = FALSE;
        memory[address] = value;
    }
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
     * Retrieve the word containing the specified byte address
     * using integer division
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
    return;
}

/*
 * Read a word of memory using it's byte address. If there is
 * an error, *memError is set to true, otherwise it's false.
 *
 * Parameters:
 *  byteAddress     a byte address of memory [0..4095] that is
 *                     a multiple of 4
 *  *memError       pointer to the memory error indicator
 *
 * Return the contents of memory at the byte address, or 0 on error
 */
unsigned int getWord(int byteAddress, bool * memError) {
    if (byteAddress % WORDSIZE) { // byteAddress not a multiple of 4
        *memError = TRUE;
        return 0;
    }

    // byteAddress is an acceptable word address
    *memError = FALSE;
    return fetch((byteAddress / WORDSIZE), memError);
}

/*
 * Store a value (1 word) in memory. If there is an error,
 * *memError is set to true, otherwise it's false. If address
 * isn't a valid memory address, then memory isn't modified.
 *
 * Parameters:
 *  byteAddress     a byte address of memory [0..4095] that is
 *                     a multiple of 4
 *  value           the value to store in memory at the address
 *  *memError       pointer to the memory error indicator
 */
void putWord(int byteAddress, unsigned int value, bool * memError) {
    if (byteAddress % WORDSIZE) { // byteAddress not a multiple of 4
        *memError = TRUE;
    } else {    // byteAddress is an acceptable word address
        *memError = FALSE;
        store((byteAddress / WORDSIZE), value, memError);
    }
}

/*
 * Set all memory locations to 0 (effectively
 * "clearing" the memory).
 */
void clearMemory() {
    clearBuffer((char *) &memory, MEMSIZE);
}

