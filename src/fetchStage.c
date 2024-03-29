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
static unsigned int predictPC(icode_t icode, unsigned int valC,
    unsigned int valP);
static unsigned int pcIncrement(unsigned int f_pc, icode_t icode);
static icode_t getIcode(uint8_t memByte, bool memError);
static uint8_t getIfun(uint8_t memByte, bool memError);
static bool instructionValid(icode_t icode);
static bool needRegids(icode_t icode);
static bool needValC(icode_t icode);
static unsigned int getValC(unsigned int f_pc, bool *memError);
static rregister getRegA(uint8_t memByte, bool memError);
static rregister getRegB(uint8_t memByte, bool memError);
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
fetchStage(const forwardType * const fwd)
{
	unsigned int f_pc;
	stat_t stat = s_okay;
	icode_t icode;
	uint8_t ifun;
	rregister rA = r_none;
	rregister rB = r_none;
	unsigned int valC = 0;	/* constant word: part of instruction */
	unsigned int valP = 0;	/* address of next sequential instruction in memory */

	f_pc = selectPC(fwd, &F);
	if (f_pc > HIGHBYTE) {
		log_info("PC problem: addr %08x is too large", f_pc);
	}

	bool memError;
	uint8_t data = getByte(f_pc, &memError);
	if (memError) {
		stat = s_addr;
	}

	icode = getIcode(data, memError);
	ifun = getIfun(data, memError);

	if (memError) {
		/*
		 * If there was a memory error fetching the instruction,
		 * icode will be NOP and stat will be SADR. So skip the
		 * other memory reading steps.
		 */
		goto updateregs;
	}

	if (!instructionValid(icode)) {
		stat = s_inst;
		F.predPC = F.predPC + 1;
		goto updateregs;
	}

	if (icode_is(icode, HALT)) {
		stat = s_halt;
	}

	// Get register ids if necessary
	if (needRegids(icode)) {
		// try to get rA and rB from the next byte of memory
		data = getByte(f_pc + 1, &memError);
		if (memError) {
			stat = s_addr;
		}

		rA = getRegA(data, memError);
		rB = getRegB(data, memError);
	}

	// Get valC if necessary
	if (needValC(icode)) {
		unsigned int tempPC = f_pc;

		if (needRegids(icode)) {
			tempPC += 1;
		}

		valC = getValC(tempPC, &memError);
		if (memError) {
			stat = s_addr;
		}
	}

updateregs:
	/* Calculate valP */
	valP = pcIncrement(f_pc, icode);

	if (stallF(fwd)) {
		/* Stall F? Just keep previous address */
		F.predPC = f_pc;
	}
	else {
		unsigned int pc = predictPC(icode, valC, valP);
		if (pc > HIGHBYTE) {
			log_debug("predicted PC problem: addr %08x will be too large",
			    f_pc);
		}
		F.predPC = pc;
	}

	if (bubbleD(fwd)) {
		/* Insert a NOP */
		updateDregister(s_okay, i_nop, FNONE, r_none, r_none, 0, 0);
	}
	else if (!stallD(fwd)) {
		/* Update D as normal */
		updateDregister(stat, icode, ifun, rA, rB, valC, valP);
	}
	/* else do nothing because D should be stalled */
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
	if (icode_is(fwd->M_icode, JXX) && !fwd->M_Cnd) {
		return fwd->M_valA;
	}

	// Completion of RET instruction
	if (icode_is(fwd->W_icode, RET)) {
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
predictPC(icode_t icode, unsigned int valC, unsigned int valP)
{
	if (icode_is(icode, JXX) || icode_is(icode, CALL)) {
		return valC;
	}

	return valP;
}

/*
 * Determine the address of the next sequential instruction in
 * memory.
 *
 * Parameters:
 *	f_pc     current value of the PC
 *	icode    the instruction code
 *
 * Return the address of the next sequential instruction (valP).
 */
unsigned int
pcIncrement(unsigned int f_pc, icode_t icode)
{
	unsigned int valP;

	switch (icode.ic) {
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
 *	memByte     a byte that should contain the instruction code
 *	memError    specifies whether a memory error occurred
 *
 * Return the decoded instruction code, or a NOP for memory error
 */
icode_t
getIcode(uint8_t memByte, bool memError)
{
	icode_t icode = i_nop;
	if (memError) {
		return icode;
	}

	icode.ic = getBits(4, 7, memByte);
	return icode;
}

/*
 * Determine the ifun of the fetched instruction.
 *
 * Parameters:
 *	memByte     a byte that should contain the instruction function
 *	memError    specifies whether a memory error occurred
 *
 * Return the instruction function, or FNONE for a memory error
 */
uint8_t
getIfun(uint8_t memByte, bool memError)
{
	if (memError) {
		return FNONE;
	}

	uint8_t ifun = getBits(0, 3, memByte);
	return ifun;
}

/*
 * Decipher program register id A.
 *
 * Parameters:
 *	memByte     a byte that should contain a program register id
 *	memError    specifies whether a memory error occurred
 */
rregister
getRegA(uint8_t memByte, bool memError)
{
	rregister rA = r_none;
	if (memError) {
		return rA;
	}

	unsigned int reg = getBits(4, 7, memByte);
	rA.reg = reg;
	return rA;
}

/*
 * Decipher program register id B.
 *
 * Parameters:
 *	memByte     a byte that should contain a program register id
 *	memError    specifies whether a memory error occurred
 */
rregister
getRegB(uint8_t memByte, bool memError)
{
	rregister rB = r_none;
	if (memError) {
		return rB;
	}

	unsigned int reg = getBits(0, 3, memByte);
	rB.reg = reg;
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
instructionValid(icode_t icode)
{
	switch (icode.ic) {
	case HALT:
	case NOP:
	case RRMOVL:
	case IRMOVL:
	case RMMOVL:
	case MRMOVL:
	case OPL:
	case JXX:
	case CALL:
	case RET:
	case PUSHL:
	case POPL:
	case DUMP:
		return TRUE;

	default:
		log_debug("invalid instruction: %x", icode.ic);
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
needRegids(icode_t icode)
{
	switch (icode.ic) {
	case RRMOVL:
	case IRMOVL:
	case RMMOVL:
	case MRMOVL:
	case OPL:
	case PUSHL:
	case POPL:
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
needValC(icode_t icode)
{
	switch (icode.ic) {
	case IRMOVL:
	case RMMOVL:
	case MRMOVL:
	case JXX:
	case CALL:
	case DUMP:
		return TRUE;

	default:
		return FALSE;
	}
}

/*
 * Get and build the constant word, valC, from memory.
 *
 * Parameters:
 *	f_pc      the current value of the PC
 *	*memError pointer to memory error status
 *
 * Return the constant word, valC.
 */
unsigned int
getValC(unsigned int f_pc, bool *memError)
{
	uint8_t byte0, byte1, byte2, byte3;

	byte0 = getByte(f_pc + 1, memError);
	if (*memError) {
		log_info("memory error in getValC!");
		return 0;
	}

	byte1 = getByte(f_pc + 2, memError);
	if (*memError) {
		log_info("memory error in getValC!");
		return 0;
	}

	byte2 = getByte(f_pc + 3, memError);
	if (*memError) {
		log_info("memory error in getValC!");
		return 0;
	}

	byte3 = getByte(f_pc + 4, memError);
	if (*memError) {
		log_info("memory error in getValC!");
		return 0;
	}

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

	if (((icode_is(fwd->E_icode, MRMOVL) || icode_is(fwd->E_icode, POPL)) &&
	    (fwd->E_dstM.reg == fwd->d_srcA.reg || fwd->E_dstM.reg == fwd->d_srcB.reg))
	    ||
	    (icode_is(fwd->D_icode, RET) || icode_is(fwd->E_icode, RET) || icode_is(fwd->M_icode, RET))) {
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
	if ((icode_is(fwd->E_icode, JXX) && !fwd->e_Cnd)
	    ||
	    (!stallD(fwd) && (icode_is(fwd->D_icode, RET) || icode_is(fwd->E_icode, RET) ||
	    icode_is(fwd->M_icode, RET)))) {
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
	if ((icode_is(fwd->E_icode, MRMOVL) || icode_is(fwd->E_icode, POPL))
	    &&
	    (fwd->E_dstM.reg == fwd->d_srcA.reg || fwd->E_dstM.reg == fwd->d_srcB.reg)) {
		stall = TRUE;
	}

	return stall;
}
