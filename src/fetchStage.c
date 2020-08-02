/*
 * Copyright (c) 2014, 2020 Scott Bennett <scottb@fastmail.com>
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

#include <stdint.h>

#include "fetchStage.h"
#include "bool.h"
#include "tools.h"
#include "memory.h"
#include "registers.h"
#include "instructions.h"
#include "decodeStage.h"

#include "logger.h"

/*
 * F register holds the input for the fetch stage.
 */
static struct fregister F;

static unsigned int selectPC(const forwardType *, const struct fregister *);
static unsigned int predictPC(uint8_t icode, unsigned int valC,
    unsigned int valP);
static unsigned int pcIncrement(unsigned int f_pc, uint8_t icode);
static bool instructionValid(uint8_t icode);
static uint8_t getIcode(uint8_t memByte, bool memError);
static unsigned int getIfun(uint8_t memByte, bool memError);
static bool needRegids(uint8_t icode);
static rregister getRegA(uint8_t memByte);
static rregister getRegB(uint8_t memByte);
static bool needValC(uint8_t icode);
static unsigned int getValC(unsigned int f_pc, bool * memError);
static bool bubbleF(void);
static bool stallF(const forwardType *);
static bool bubbleD(const forwardType *);
static bool stallD(const forwardType *);

/*
 * Return a copy of the F register.
 *
 * Return an fregister
 */
struct fregister
getFregister()
{
	return F;
}

/*
 * Clear the F register
 */
void
clearFregister()
{
	F.predPC = 0;
}

/*
 * Fetch an instruction from memory and update the D register
 * accordingly.
 *
 * Parameters:
 *	*fwd     holds values forwarded from previous stages
 */
void
fetchStage(const forwardType *fwd)
{
	unsigned int f_pc;
	stat_t stat = { SAOK };
	uint8_t icode;
	unsigned int ifun;
	rregister rA = { RNONE };
	rregister rB = { RNONE };
	unsigned int valC = 0;	/* constant word: part of instruction */
	unsigned int valP = 0;	/* address of next sequential instruction in memory */

	f_pc = selectPC(fwd, &F);
	if (f_pc > HIGHBYTE) {
		log_info("PC problem: addr %08x is too large", f_pc);
	}

	bool memError;
	uint8_t data = getByte(f_pc, &memError);

    if (memError) {
        stat.s = SADR;
    }

    icode = getIcode(data, memError);
    ifun = getIfun(data, memError);

    if (instructionValid(icode)) {
        if (icode == HALT) {
            stat.s = SHLT;
        }

        // Get register ids if necessary
        if (needRegids(icode)) {
            unsigned int tempPC = f_pc + 1;

            // get rA and rB from the next byte of memory
            data = getByte(tempPC, &memError);

            if (memError) {
                stat.s = SADR;
            }

		rA = getRegA(data);
		rB = getRegB(data);
        }

        // Get valC if necessary
        if (needValC(icode)) {
            unsigned int tempPC = f_pc;

            if (needRegids(icode)) {
                tempPC += 1;
            }

            valC = getValC(tempPC, &memError);

            if (memError) {
                stat.s = SADR;
            }
        }
    }
    else {
        stat.s = SINS;
        F.predPC = F.predPC + 1;
    }

    // Calculate valP
    valP = pcIncrement(f_pc, icode);

    // Stall F? keep previous value
    if (stallF(fwd)) {
        F.predPC = f_pc;
    }
    else {
        F.predPC = predictPC(icode, valC, valP);
    }

    // Stall or bubble D?
    if (bubbleD(fwd)) {
        // Insert a NOP
	stat_t okay = { SAOK };
	rregister rnone = { RNONE };
        updateDregister(okay, NOP, 0, rnone, rnone, 0, 0);
    }
    else if (!stallD(fwd)) {
        // Update D as normal (do not stall)
        updateDregister(stat, icode, ifun, rA, rB, valC, valP);
    }
	// else do nothing because D should be stalled
}

/*
 * Select the source of the PC.
 *
 * Parameters:
 *	*fwd     holds values forwarded from previous stages
 *	predPC   current value of F.predPC
 *
 * Return source value for the PC
 */
unsigned int
selectPC(const forwardType *fwd, const struct fregister *freg)
{
    // Uses forwarded M_valA, W_valM
    // Mispredicted branch. Fetch at incremented PC
    if (fwd->M_icode == JXX && !(fwd->M_Cnd)) {
        return fwd->M_valA;
    }

    // Completion of RET instruction
    if (fwd->W_icode == RET) {
        return fwd->W_valM;
    }

    // Default
    return freg->predPC;
}

/*
 * Predict the value of the next instruction to be
 * executed.
 *
 * Parameters:
 *	icode     the instruction code
 *	valC      a constant word, part of the instruction
 *	valP      the address of next sequential instruction
 *                in memory
 *
 * Return the predicted PC.
 */
unsigned int
predictPC(uint8_t icode, unsigned int valC, unsigned int valP)
{
	if (icode == JXX || icode == CALL) {
		return valC;
	}
	else {
		return valP;
	}
}

/*
 * Determine the address of the next sequential instruction in
 * memory.
 *
 * Parameters:
 *	f_pc     current value of the PC
 *	icode    the instruction code
 *
 * Return the address of the next sequential instruction.
 */
unsigned int
pcIncrement(unsigned int f_pc, uint8_t icode)
{
    unsigned int valP;

    switch (icode) {
        case HALT:
        case NOP:
        case RET:
            valP = f_pc + 1;
            break;

        case OPL:
        case RRMOVL:
        case POPL:
        case PUSHL:
            valP = f_pc + 2;
            break;

        case DUMP:
        case JXX:
        case CALL:
            valP = f_pc + 5;
            break;

        case IRMOVL:
        case RMMOVL:
        case MRMOVL:
            valP = f_pc + 6;
            break;

        default:
            valP = f_pc + 1;
    }

    return valP;
}

/*
 * Determine the icode of the fetched instruction.
 *
 * Parameters:
 *  memByte     a byte that should contain the instruction code
 *  memError    specifies whether a memory error occurred
 *
 * Return the instruction code, or a NOP for a memory error
 */
uint8_t
getIcode(uint8_t memByte, bool memError)
{
	if (memError) {
		return NOP;
	}

	uint8_t icode = getBits(4, 7, memByte);
	return icode;
}

/*
 * Determine the ifun of the fetched instruction.
 *
 * Parameters:
 *  memByte     a byte that should contain the instruction function
 *  memError    specifies whether a memory error occurred
 *
 * Return the instruction function, or FNONE for a memory error
 */
unsigned int
getIfun(uint8_t memByte, bool memError)
{
    if (memError) {
        return 0;
    } else {
        return getBits(0, 3, memByte);
    }
}

/*
 * Decipher program register id A.
 *
 * Parameters:
 *  memByte     a byte that should contain a program register id
 */
rregister
getRegA(uint8_t memByte)
{
	unsigned int reg = getBits(4, 7, memByte);
	rregister rA = { reg };
	return rA;
}

/*
 * Decipher program register id B.
 *
 * Parameters:
 *  memByte     a byte that should contain a program register id
 */
rregister
getRegB(uint8_t memByte)
{
	unsigned int reg = getBits(0, 3, memByte);
	rregister rB = { reg };
	return rB;
}

/*
 * Test whether the fetched instruction is valid.
 *
 * Parameters:
 *	icode     the instruction code
 *
 * Return true if icode is valid.
 */
bool
instructionValid(uint8_t icode)
{
    switch (icode) {
        case HALT:
        case NOP:
        case DUMP:
        case IRMOVL:
        case OPL:
        case RRMOVL:
        case RMMOVL:
        case MRMOVL:
        case POPL:
        case PUSHL:
        case RET:
        case JXX:
        case CALL:
		return TRUE;

        default:
		log_debug("invalid instruction: %02x", icode);
		return FALSE;
    }
}

/*
 * Does fetched instruction require a regid byte?
 *
 * Parameters:
 *	icode     the instruction code
 *
 * Return true if icode requires a regid.
 */
bool
needRegids(uint8_t icode)
{
    switch (icode) {
        case RRMOVL:
        case OPL:
        case PUSHL:
        case POPL:
        case IRMOVL:
        case RMMOVL:
        case MRMOVL:
		return TRUE;

        default:
		return FALSE;
    }
}

/*
 * Does fetched instruction require a constant word (valC)?
 *
 * Parameters:
 *	icode     the instruction code
 *
 * Return true if instruction needs a valC.
 */
bool
needValC(uint8_t icode)
{
    switch (icode) {
        case DUMP:
        case IRMOVL:
        case RMMOVL:
        case MRMOVL:
        case JXX:
        case CALL:
		return TRUE;

        default:
		return FALSE;
    }
}

/*
 * Get and build the constant word, valC, from memory.
 *
 * Parameters:
 *  f_pc        the current value of the PC
 *  *memError   pointer to memory error status
 *
 * Return the constant word, valC
 */
unsigned int
getValC(unsigned int f_pc, bool * memError)
{
    unsigned char byte0, byte1, byte2, byte3;

    byte0 = getByte(f_pc + 1, memError);
    byte1 = getByte(f_pc + 2, memError);
    byte2 = getByte(f_pc + 3, memError);
    byte3 = getByte(f_pc + 4, memError);

    return buildWord(byte0, byte1, byte2, byte3);
}

/*
 * Determine if F should be bubbled.
 * According to HCL, F will never be bubbled, therefore always
 * return false.
 */
bool
bubbleF()
{
    return FALSE;
}

/*
 * Determine if F should be stalled based on input forwarded
 * from later stages.
 *
 * Parameters:
 *	*fwd     holds values from later stages
 *
 * Return true if F needs to be stalled.
 */
bool
stallF(const forwardType *fwd)
{
	bool stall = FALSE;

	if (((fwd->E_icode == MRMOVL || fwd->E_icode == POPL) &&
	    (fwd->E_dstM.reg == fwd->d_srcA.reg || fwd->E_dstM.reg == fwd->d_srcB.reg))
	    ||
	    (fwd->D_icode == RET || fwd->E_icode == RET || fwd->M_icode == RET)) {
		stall = TRUE;
	}

	return stall;
}

/*
 * Determine if D should be bubbled based on input forwarded
 * from later stages.
 *
 * Parameters:
 *	*fwd     holds values from later stages
 *
 * Return true if D should be bubbled.
 */
bool
bubbleD(const forwardType *fwd)
{
	bool bubble = FALSE;

	// conditions for mispredicted branch
	if ((fwd->E_icode == JXX && !fwd->e_Cnd)
	    ||
	    (!stallD(fwd) && (RET == fwd->D_icode || RET == fwd->E_icode
	     || RET == fwd->M_icode))) {
		bubble = TRUE;
	}

	return bubble;
}

/*
 * Determine if D should be stalled based on input forwarded
 * from later stages.
 *
 * Parameters:
 *	*fwd     holds values from later stages
 *
 * Return true if D should be stalled.
 */
bool
stallD(const forwardType *fwd)
{
	bool stall = FALSE;

	// conditions for load/use hazard
	if ((fwd->E_icode == MRMOVL || fwd->E_icode == POPL)
	    &&
	    (fwd->E_dstM.reg == fwd->d_srcA.reg || fwd->E_dstM.reg == fwd->d_srcB.reg)) {
		stall = TRUE;
	}

	return stall;
}
