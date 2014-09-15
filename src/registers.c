/* 
 * File:   registers.c
 * Author: Scott Bennett
 */

#include "bool.h"
#include "registers.h"
#include "tools.h"

static unsigned int registers[REGSIZE];
static unsigned int CC;   // Condition code

/* 
 * Returns the value in the specified register.
 * 
 * Parameters:
 *  regNum  register number
 * 
 * Return the value in the register. In case of error,
 *  return 0
 */
unsigned int getRegister(int regNum) {
    if (regNum >= 0 && regNum < REGSIZE) {
        return registers[regNum];
    } else {
        return 0;
    }
}

/* 
 * Set register regNum to the value regValue.
 *
 * @param regNum   Register number
 * @param regValue Value to put in the register
 */
void setRegister(int regNum, unsigned int regValue) {
    if (regNum >= 0 || regNum < REGSIZE)
        registers[regNum] = regValue;
}

/**
 * Clear the program registers.
 */
void clearRegisters() {
    clearBuffer((char *) &registers, REGSIZE);
}

/**
 * Set a bit in the condition code register (CC).
 *
 * Sets bit number of CC specified by bitNumber param to 
 * value in value param.
 * 
 * @param bitNumber  The condition code to set
 * @param value      Value to set the condition code
 */
void setCC(unsigned int bitNumber, unsigned int value) {
    if (bitNumber == ZF || bitNumber == SF || bitNumber == OF) {
        CC = assignOneBit(bitNumber, value, CC);
    }
}

/**
 * Return a bit in the condition code register (CC).
 *
 * Returns the value of the bitNumber bit in CC.
 * 
 * @param bitNumber  The condition code to get
 * @return The value at bitNumber
 */
unsigned int getCC(unsigned int bitNumber){    
    if (bitNumber == ZF || bitNumber == SF || bitNumber == OF) {
        return getBits(bitNumber, bitNumber, CC);
    }
}

/**
 * Clear the condition code register.
 */
void clearCC() {
    CC = 0;
}
