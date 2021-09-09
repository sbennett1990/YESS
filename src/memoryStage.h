/*
 * File:   memoryStage.h
 * Author: Alex Savarda
 */

#ifndef MEMORYSTAGE_H
#define	MEMORYSTAGE_H

#include "forwarding.h"
#include "instructions.h"
#include "registers.h"

/*
 * +----+---------------------------------------------------------------------+
 * | M  | icode |    | Cnd |    |  valE  |  valA  |       |dstE|dstM|         |
 * +----+---------------------------------------------------------------------+
 */
struct mregister {
	stat_t stat;
	icode_t icode;
	unsigned int Cnd;
	unsigned int valE;	// Result from Execute Stage (ALU)
	unsigned int valA;	// Data to be written to memory (except POPL & RET)
	rregister dstE;		// Program Register: destination for valE
	rregister dstM;		// Program Register: destination for valM
};

struct mregister getMregister(void);
void clearMregister(void);
void updateMRegister(stat_t stat, icode_t icode, unsigned int Cnd,
    unsigned int valE, unsigned int valA, rregister dstE, rregister dstM);
void memoryStage(forwardType *);

#endif	/* MEMORYSTAGE_H */
