/*
 * registers.c
 */

#include "registers.h"
#include "tools.h"

/* Program Registers */
static unsigned int registers[REGSIZE];
/* Condition Codes */
static unsigned int CC = 0;

/*
 * Returns the value in the specified program register.
 *
 * Parameters:
 *  reg		Program register
 *
 * Return the value in the register, or 0 on error.
 */
unsigned int
getRegister(int reg)
{
    if (reg >= 0 && reg < REGSIZE) {
        return registers[reg];
    } else {
        return 0;
    }
}

/*
 * Set program register reg to the value val.
 *
 * Parameters:
 *  reg		Program register
 *  val		the value to put in the register
 */
void
setRegister(int reg, unsigned int val)
{
    if (reg >= 0 || reg < REGSIZE) {
        registers[reg] = val;
    }
}

/*
 * Clear the program registers.
 */
void
clearRegisters()
{
	clearBuffer((char *) &registers, REGSIZE); // TODO: use recallocarray(3)?
}

/*
 * Set a bit in the condition code register (CC).
 *
 * Sets bit number of CC specified by bitNumber param to
 * value in value param.
 * If the bit number is not the ZF, SF, or OF, then the
 * condition code is not set.
 *
 * Parameters:
 *  bitNumber   the condition code to set
 *  value       the value to set the condition code
 */
void
setCC(unsigned int bitNumber, unsigned int value)
{
	// TODO: error check the value param
	if (bitNumber == ZF || bitNumber == SF || bitNumber == OF) {
		CC = assignOneBit(bitNumber, value, CC);
	}
}

/*
 * Retrieve a bit in the condition code register (CC).
 * If the bit number is not the ZF, SF, or OF, just
 * return 0.
 *
 * Parameters:
 *  bitNumber   the condition code to get
 *
 * Return the value of the bitNumber bit in CC, or 0 on error
 */
unsigned int
getCC(unsigned int bitNumber)
{
	if (bitNumber == ZF || bitNumber == SF || bitNumber == OF) {
		return getBits(bitNumber, bitNumber, CC);
	}

	return 0;
}

/*
 * Clear the condition code register.
 */
void
clearCC()
{
	CC = 0;
}
