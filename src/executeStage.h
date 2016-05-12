/*
 * File:   executeStage.h
 * Author: Alex Savarda
 */

#define INSTR_COUNT     16   // Possible size of the instruction set

#ifndef EXECUTESTAGE_H
#define EXECUTESTAGE_H

typedef struct {
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
} eregister;

struct E {
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
eregister getEregister(void);
void clearEregister(void);
void executeStage();
void initFuncPtrArray(void);
void updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun,
                     unsigned int valC, unsigned int valA, unsigned int valB, 
                     unsigned int dstE, unsigned int dstM, unsigned int srcA, 
                     unsigned int srcB);
#endif	/* EXECUTESTAGE_H */

