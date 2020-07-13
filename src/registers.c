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
getRegister(rregister reg)
{
	if (reg.reg >= 0 && reg.reg < REGSIZE) {
		return registers[reg.reg];
	}
	else {
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
setRegister(rregister reg, unsigned int val)
{
	if (reg.reg >= 0 || reg.reg < REGSIZE) {
		registers[reg.reg] = val;
	}
}

/*
 * Clear the program registers.
 */
void
clearRegisters()
{
	clearBuffer((char *) &registers, REGSIZE);
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
