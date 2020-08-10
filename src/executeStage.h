/*
 * File:   executeStage.h
 * Author: Alex Savarda
 */

#ifndef EXECUTESTAGE_H
#define EXECUTESTAGE_H

#include "forwarding.h"
#include "instructions.h"
#include "registers.h"

/*
 * +----+---------------------------------------------------------------------+
 * | E  | icode | ifun  |      |  valC  |  valA  |  valB  |dstE|dstM|srcA|srcB|
 * +----+---------------------------------------------------------------------+
 */
struct eregister {
	stat_t stat;
	icode_t icode;
	unsigned int ifun;
	unsigned int valC;
	unsigned int valA;	/* Data read from srcA */
	unsigned int valB;
	rregister dstE;		/* Program Register: destination for valE */
	rregister dstM;		/* Program Register */
	rregister srcA;		/* Program Register */
	rregister srcB;		/* Program Register */
};


struct eregister getEregister(void);
void clearEregister(void);
void executeStage(forwardType *);
void initFuncPtrArray(void);
void updateEregister(stat_t stat, icode_t icode, unsigned int ifun,
    unsigned int valC, unsigned int valA, unsigned int valB, rregister dstE,
    rregister dstM, rregister srcA, rregister srcB);
#endif	/* EXECUTESTAGE_H */
