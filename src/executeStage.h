/*
 * File:   executeStage.h
 * Author: Alex Savarda
 */

#ifndef EXECUTESTAGE_H
#define EXECUTESTAGE_H

#include "forwarding.h"
#include "control.h"
#include "status.h"

/*
 * +----+---------------------------------------------------------------------+
 * | E  | icode | ifun  |      |  valC  |  valA  |  valB  |dstE|dstM|srcA|srcB|
 * +----+---------------------------------------------------------------------+
 */
struct eregister {
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int valC;
    unsigned int valA;
    unsigned int valB;
    unsigned int dstE;
    unsigned int dstM;
    unsigned int srcA;
    unsigned int srcB;
};


// Function prototypes
struct eregister getEregister(void);
void clearEregister(void);
void executeStage(forwardType *, statusType, controlType *);
void initFuncPtrArray(void);
void updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun,
                     unsigned int valC, unsigned int valA, unsigned int valB,
                     unsigned int dstE, unsigned int dstM, unsigned int srcA,
                     unsigned int srcB);
#endif	/* EXECUTESTAGE_H */

