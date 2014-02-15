/** 
 * File:   writebackStage.c
 * Author: Alex Savarda
 */

#include <stdio.h>
#include "bool.h"
#include "tools.h"
#include "dump.h"
#include "instructions.h"
#include "status.h"
#include "control.h"
#include "forwarding.h"
#include "writebackStage.h"
#include "executeStage.h"
#include "decodeStage.h"


//W register holds the input for the writeback stage.
//It is only accessible from this file (static)
static wregister W;

/**
 * Return a copy of the W register
 */
wregister getWregister() {
    return W;
}

/**
 * Clear W register then initialize its icode to NOP and
 * its stat to SAOK.
 */
void clearWregister() {
    clearBuffer((char *) &W, sizeof(W));
    W.icode = NOP;
    W.stat = SAOK;
}

/**
 * Writes up to two results to the register file.
 * Check the current 'stat' for SINS, SADR, and SHLT to HALT or continue
 * program. When an exception is encountered, writeback will return TRUE.
 * 
 * @param *forward Holds values forwarded to previous stages
 * @param *status  Holds values of statuses
 * @return False to continue program, true if an exception is encountered
 */
bool writebackStage(forwardType * forward, statusType * status) {
	//if stat == SINS, SADR, SHLT; HALT program and dump
    if (W.stat == SINS) {
        printf("Invalid instruction\n");
        dumpProgramRegisters();
        dumpProcessorRegisters();
        dumpMemory();
        return TRUE;
    }
    else if (W.stat == SADR) {
        printf("Invalid memory address\n");
        dumpProgramRegisters();
        dumpProcessorRegisters();
        dumpMemory();
        return TRUE;
    }
    else if (W.stat == SHLT) {
        return TRUE;
    }

	//if icode = DUMP, dump appropriate information
    if(W.icode == DUMP && getBits(0, 0, W.valE))
        dumpProgramRegisters();

    if(W.icode == DUMP && getBits(1, 1, W.valE))
        dumpProcessorRegisters();

    if(W.icode == DUMP && getBits(2, 2, W.valE))
        dumpMemory();

    
    //set fields of forward and status struct to current values
    forward->W_dstE = W.dstE;
    forward->W_valE = W.valE;
    forward->W_dstM = W.dstM;
    forward->W_valM = W.valM;
    forward->W_icode = W.icode;
    status->W_stat = W.stat;
    
	//write result to appropriate register
    setRegister(W.dstE, W.valE);
    setRegister(W.dstM, W.valM);

    return FALSE;
}

/**
 * Update the values in the W register
 */
void updateWRegister(unsigned int stat, unsigned int icode, unsigned int valE,
                     unsigned int valM, unsigned int dstE, unsigned int dstM) {
    W.stat = stat;
    W.icode = icode;
    W.valE = valE;
    W.valM = valM;
    W.dstE = dstE;
    W.dstM = dstM;
}
