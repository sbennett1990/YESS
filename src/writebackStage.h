/*
 * File:   writebackStage.h
 */

#ifndef	WRITEBACKSTAGE_H
#define	WRITEBACKSTAGE_H

#include "forwarding.h"
#include "status.h"

/*
 * +----+---------------------------------------------------------------------+
 * | W  | icode |               |  valE  |  valM  |       |dstE|dstM|         |
 * +----+---------------------------------------------------------------------+
 */
struct wregister {
	unsigned int stat;
	unsigned int icode;
	unsigned int valE;
	unsigned int valM;
	unsigned int dstE;
	unsigned int dstM;
};

int writebackStage(forwardType *, statusType *);
struct wregister getWregister(void);
void clearWregister(void);
void updateWRegister(unsigned int stat, unsigned int icode, unsigned int valE,
	unsigned int valM, unsigned int dstE, unsigned int dstM);

#endif	/* WRITEBACKSTAGE_H */
