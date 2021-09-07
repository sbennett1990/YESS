/*
 * File:   writebackStage.c
 * Author: Alex Savarda
 */

#include <stdio.h>

#include "tools.h"
#include "dump.h"
#include "instructions.h"
#include "writebackStage.h"

#include "logger.h"

/*
 * W register holds the input for the write back stage.
 */
static struct wregister W;

/*
 * Return a copy of the W register.
 */
struct wregister
getWregister()
{
	return W;
}

/*
 * Clear the W register then initialize its icode to NOP and
 * its stat to SAOK.
 */
void
clearWregister()
{
	W.stat = s_okay;
	W.icode = i_nop;
	W.valE = 0;
	W.valM = 0;
	W.dstE = r_none;
	W.dstM = r_none;
}

/*
 * Writes up to two results to the register file.
 * Check the current 'stat' for SINS, SADR, and SHLT to HALT or continue
 * program. When an exception is encountered, writeback will return -1.
 *
 * Parameters:
 *	*fwd    Holds values forwarded to previous stages
 *
 * Returns -1 if an exception occurs, and >0 to
 * continue program execution.
 */
int
writebackStage(forwardType *fwd, int *dump)
{
	// if stat == SINS, SADR, SHLT; HALT program and dump
	if (W.stat.s == SINS) {
		printf("\n*** Invalid instruction ***\n");
		dumpProgramRegisters();
		dumpProcessorRegisters();
		dumpMemory();
		*dump = 1;
		return -1;
	}
	else if (W.stat.s == SADR) {
		printf("\n*** Invalid memory address ***\n");
		dumpProgramRegisters();
		dumpProcessorRegisters();
		dumpMemory();
		*dump = 1;
		return -1;
	}
	else if (W.stat.s == SHLT) {
		log_debug("halting program");
		return -1; // XXX: maybe return a different code to differentiate?
	}

	// if icode = DUMP, dump appropriate information
	if (icode_is(W.icode, DUMP)) {
		if (getBits(0, 0, W.valE)) {
			dumpProgramRegisters();
			*dump = 1;
		}
		if (getBits(1, 1, W.valE)) {
			dumpProcessorRegisters();
			*dump = 1;
		}
		if (getBits(2, 2, W.valE)) {
			dumpMemory();
			*dump = 1;
		}
	}

	// set fields of forward struct to current values
	fwd->W_stat = W.stat;
	fwd->W_icode = W.icode;
	fwd->W_dstE = W.dstE;
	fwd->W_valE = W.valE;
	fwd->W_dstM = W.dstM;
	fwd->W_valM = W.valM;

	// write result to appropriate program register
	setRegister(W.dstE, W.valE);
	setRegister(W.dstM, W.valM);

	return 1;
}

/*
 * Update the values in the W register.
 *
 * Parameters:
 *	stat     Status (?)
 *	icode    Instruction Code from the M register
 *	valE     Value destined for program register 'E'
 *	valM     Value destined for program register 'M', if applicable
 *	dstE     Destination program register 'E', from the M register
 *	dstM     Destination program register 'M', from the M register
 */
void
updateWRegister(stat_t stat, icode_t icode, unsigned int valE,
    unsigned int valM, rregister dstE, rregister dstM)
{
	W.stat = stat;
	W.icode = icode;
	W.valE = valE;
	W.valM = valM;
	W.dstE = dstE;
	W.dstM = dstM;
}
