/*
 * File:   memoryStage.h
 * Author: Alex Savarda
 */

#ifndef MEMORYSTAGE_H
#define	MEMORYSTAGE_H

#include "forwarding.h"
#include "control.h"
#include "status.h"

typedef struct {
    unsigned int stat;
    unsigned int icode;
    unsigned int Cnd;
    unsigned int valE;
    unsigned int valA;
    unsigned int dstE;
    unsigned int dstM;
} mregister;

/*
 * +----+---------------------------------------------------------------------+
 * | M  | icode |    | Cnd |    |  valE  |  valA  |       |dstE|dstM|         |
 * +----+---------------------------------------------------------------------+
 */
struct M {
    unsigned int stat;
    unsigned int icode;
    unsigned int Cnd;
    unsigned int valE;
    unsigned int valA;
    unsigned int dstE;
    unsigned int dstM;
};

mregister getMregister(void);
void clearMregister(void);
void memoryStage(forwardType *, statusType *, controlType *);
void updateMRegister(unsigned int stat, unsigned int icode, unsigned int Cnd,
                     unsigned int valE, unsigned int valA, unsigned int dstE,
                     unsigned int dstM);

#endif	/* MEMORYSTAGE_H */
