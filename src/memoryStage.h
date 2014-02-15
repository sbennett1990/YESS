/**
 * File:   memoryStage.h
 * Author: Alex Savarda
 */

#ifndef MEMORYSTAGE_H
#define	MEMORYSTAGE_H

typedef struct {
    unsigned int stat;
    unsigned int icode;
    unsigned int Cnd;
    unsigned int valE;
    unsigned int valA;
    unsigned int dstE;
    unsigned int dstM;
} mregister;

struct M {
    unsigned int stat;
    unsigned int icode;
    unsigned int Cnd;
    unsigned int valE;
    unsigned int valA;
    unsigned int dstE;
    unsigned int dstM;
};

//prototypes for functions called from files other than memoryStage
mregister getMregister(void);
void memoryStage();
void clearMregister(void);
void updateMRegister(unsigned int stat, unsigned int icode, unsigned int Cnd,
                     unsigned int valE, unsigned int valA, unsigned int dstE, 
                     unsigned int dstM);
#endif	/* MEMORYSTAGE_H */

