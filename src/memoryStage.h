/*
 * File:   memoryStage.h
 * Author: Alex Savarda
 */

#ifndef MEMORYSTAGE_H
#define	MEMORYSTAGE_H

#include "forwarding.h"
#include "control.h"
#include "status.h"
#include "registers.h"

/*
 * +----+---------------------------------------------------------------------+
 * | M  | icode |    | Cnd |    |  valE  |  valA  |       |dstE|dstM|         |
 * +----+---------------------------------------------------------------------+
 */
struct mregister {
	unsigned int stat;
	unsigned int icode;
	unsigned int Cnd;
	unsigned int valE;
	unsigned int valA;
	rregister dstE;		// Program Register
	rregister dstM;		// Program Register
};

struct mregister getMregister(void);
void clearMregister(void);
void memoryStage(forwardType *, statusType *, controlType *);
void updateMRegister(unsigned int stat, unsigned int icode, unsigned int Cnd,
    unsigned int valE, unsigned int valA, rregister dstE, rregister dstM);

#endif	/* MEMORYSTAGE_H */
