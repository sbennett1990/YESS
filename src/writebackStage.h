/*
 * File:   writebackStage.h
 */

#ifndef	WRITEBACKSTAGE_H
#define	WRITEBACKSTAGE_H

#include "forwarding.h"
#include "registers.h"

/*
 * +----+---------------------------------------------------------------------+
 * | W  | icode |               |  valE  |  valM  |       |dstE|dstM|         |
 * +----+---------------------------------------------------------------------+
 */
struct wregister {
	unsigned int stat;
	unsigned int icode;
	unsigned int valE;	// Result from Execute Stage (ALU)
	unsigned int valM;	// Data read from memory
	rregister dstE;		// Program Register: destination for valE
	rregister dstM;		// Program Register: destination for valM
};

int writebackStage(forwardType *);
struct wregister getWregister(void);
void clearWregister(void);
void updateWRegister(unsigned int stat, unsigned int icode, unsigned int valE,
    unsigned int valM, rregister dstE, rregister dstM);

#endif	/* WRITEBACKSTAGE_H */
