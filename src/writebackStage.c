/*
 * File:   writebackStage.c
 * Author: Alex Savarda
 */

#include <stdio.h>

#include "tools.h"
#include "dump.h"
#include "registers.h"
#include "instructions.h"
#include "writebackStage.h"

/*
 * W register holds the input from the memory stage.
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
	W.stat = SAOK;
	W.icode = NOP;
	W.valE = 0;
	W.valM = 0;
	W.dstE = RNONE;
	W.dstM = RNONE;
}

/*
 * Writes up to two results to the register file.
 * Check the current 'stat' for SINS, SADR, and SHLT to HALT or continue
 * program. When an exception is encountered, writeback will return -1.
 *
 * Parameters:
 *	*forward    Holds values forwarded to previous stages
 *	*status     Holds values of statuses
 *
 * Returns -1 if an exception occurs, and >0 to
 * continue program execution.
 */
int
writebackStage(forwardType *forward, statusType *status)
{
	// if stat == SINS, SADR, SHLT; HALT program and dump
	if (W.stat == SINS) {
		printf("\n*** Invalid instruction ***\n");
		dumpProgramRegisters();
		dumpProcessorRegisters();
		dumpMemory();
		return -1;
	}
	else if (W.stat == SADR) {
		printf("\n*** Invalid memory address ***\n");
		dumpProgramRegisters();
		dumpProcessorRegisters();
		dumpMemory();
		return -1;
	}
	else if (W.stat == SHLT) {
		return -1; // XXX: maybe return a different code to differentiate?
	}

	// if icode = DUMP, dump appropriate information
	if (W.icode == DUMP && getBits(0, 0, W.valE)) {
		dumpProgramRegisters();
	}

	if (W.icode == DUMP && getBits(1, 1, W.valE)) {
		dumpProcessorRegisters();
	}

	if (W.icode == DUMP && getBits(2, 2, W.valE)) {
		dumpMemory();
	}

	// set fields of forward and status struct to current values
	forward->W_dstE = W.dstE;
	forward->W_valE = W.valE;
	forward->W_dstM = W.dstM;
	forward->W_valM = W.valM;
	forward->W_icode = W.icode;
	status->W_stat = W.stat;

	// write result to appropriate program register
	rregister dstE = { W.dstE };
	setRegister(dstE, W.valE);
	rregister dstM = { W.dstM };
	setRegister(dstM, W.valM);

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
updateWRegister(unsigned int stat, unsigned int icode, unsigned int valE,
    unsigned int valM, rregister dstE, rregister dstM)
{
	W.stat = stat;
	W.icode = icode;
	W.valE = valE;
	W.valM = valM;
	W.dstE = dstE.reg;
	W.dstM = dstM.reg;
}
