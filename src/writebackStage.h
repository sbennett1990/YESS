/*
 * File:   writebackStage.h
 */

#ifndef	WRITEBACKSTAGE_H
#define	WRITEBACKSTAGE_H

#include "forwarding.h"
#include "instructions.h"
#include "registers.h"

/*
 * +----+---------------------------------------------------------------------+
 * | W  | icode |               |  valE  |  valM  |       |dstE|dstM|         |
 * +----+---------------------------------------------------------------------+
 */
struct wregister {
	stat_t stat;
	icode_t icode;
	unsigned int valE;	/* Result from Execute Stage (ALU) */
	unsigned int valM;	/* Data read from memory */
	rregister dstE;		/* Program Register: destination for valE */
	rregister dstM;		/* Program Register: destination for valM */
};

struct wregister getWregister(void);
void clearWregister(void);
void updateWRegister(stat_t stat, icode_t icode, unsigned int valE,
    unsigned int valM, rregister dstE, rregister dstM);
int writebackStage(forwardType *, int *);

#endif	/* WRITEBACKSTAGE_H */
