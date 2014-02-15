/**
 * File:   memory.c
 * Author: Scott Bennett
 */

#include "bool.h"
#include "tools.h"
#include "memory.h"

static unsigned int memory[MEMSIZE];

// Prototypes for "private" functions
static unsigned int fetch(int address, bool * memError);
static void store(int address, unsigned int value, bool * memError);

/**
 * Read and return a word of memory. If there is an error,
 * *memError is set to true, otherwise it's false.
 * 
 * @param address    Address of memory [0..1023]
 * @param *memError  Pointer to a memory error indicator
 * @return The contents of memory at address, or 0 for error
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

/**
 * Store a value (1 word) in memory. If there is an error,
 * *memError is set to true, otherwise it's false. If address
 * isn't a valid memory address, then memory isn't modified.
 * 
 * @param address    Address of memory [0..1023]
 * @param value      Value to store in memory at address
 * @param *memError  Pointer to a memory error indicator
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

/**
 * Read a byte of memory using it's byte address. If there is 
 * an error, *memError is set to true, otherwise it's false.
 * 
 * @param byteAddress  A byte address of memory [0..4095]
 * @param *memError    Pointer to a memory error indicator
 * @return The contents of memory at byte address, or 0 for error
 */
unsigned char getByte(int byteAddress, bool * memError) {
    if (byteAddress < 0 || byteAddress > HIGHBYTE) {
        *memError = TRUE;
        return 0;
    }
    
    *memError = FALSE;
    
    // Retrieve the word containing the specified byte address 
    // using integer division
    unsigned int word = fetch((byteAddress / WORDSIZE), memError);
    
    return getByteNumber((byteAddress % WORDSIZE), word);
}

/**
 * Store a value (1 byte) in memory. If there is an error,
 * *memError is set to true, otherwise it's false. If address
 * isn't a valid memory address, then memory isn't modified.
 * 
 * @param byteAddress  A byte address of memory [0..4095]
 * @param value        Value to store in memory at address
 * @param *memError    Pointer to a memory error indicator
 */
void putByte(int byteAddress, unsigned char value, bool * memError) {
    if (byteAddress < 0 || byteAddress > HIGHBYTE) {
        *memError = TRUE;
    } else {
        *memError = FALSE;
        
        // Retrieve the word containing the specified byte address 
        // using integer division
        unsigned int word = fetch((byteAddress / WORDSIZE), memError);
        
        // Modify the byte address of the word, store in newWord
        unsigned int newWord = putByteNumber((byteAddress % WORDSIZE), value, word);
        
        store((byteAddress / WORDSIZE), newWord, memError);
    }
}

/**
 * Read a word of memory using it's byte address. If there is 
 * an error, *memError is set to true, otherwise it's false.
 * 
 * @param byteAddress  A byte address of memory [0..4095] that is
 *                     a multiple of 4
 * @param *memError    Pointer to a memory error indicator
 * @return The contents of memory at byte address, or 0 for error
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

/**
 * Store a value (1 word) in memory. If there is an error,
 * *memError is set to true, otherwise it's false. If address
 * isn't a valid memory address, then memory isn't modified.
 * 
 * @param byteAddress  A byte address of memory [0..4095] that is
 *                     a multiple of 4
 * @param value        Value to store in memory at address
 * @param *memError    Pointer to a memory error indicator
 */
void putWord(int byteAddress, unsigned int value, bool * memError) {
    if (byteAddress % WORDSIZE) { // byteAddress not a multiple of 4
        *memError = TRUE;
    } else {  // byteAddress is an acceptable word address
        *memError = FALSE;
        store((byteAddress / WORDSIZE), value, memError);
    }
}

/**
 * Set all memory locations to 0 (effectively
 * "clearing" the memory).
 */
void clearMemory() {
    clearBuffer((char *) &memory, MEMSIZE);
}

