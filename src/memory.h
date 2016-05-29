/* 
 * File:   memory.h
 * Author: Scott Bennett
 */

#define MEMSIZE     1024    // 1024 words of memory
#define WORDSIZE       4    // Size of one word in bytes
#define HIGHBYTE    4095    // Highest byte address in memory
#define NOADDRESS      0    // Value for a "no address"

#ifndef MEMORY_H
#define	MEMORY_H

unsigned char getByte(int address, bool * memError);
void putByte(int address, unsigned char value, bool * memError);
void clearMemory(void);
unsigned int getWord(int address, bool * memError);
void putWord(int address, unsigned int value, bool * memError);

#endif	/* MEMORY_H */
