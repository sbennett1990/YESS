/* 
 * File:   writebackStage.h
 * Author: Scott Bennett
 */

#ifndef WRITEBACKSTAGE_H
#define	WRITEBACKSTAGE_H

typedef struct {
    unsigned int stat;
    unsigned int icode;
    unsigned int valE;
    unsigned int valM;
    unsigned int dstE;
    unsigned int dstM;
} wregister;


struct W {
    unsigned int stat;
    unsigned int icode;
    unsigned int valE;
    unsigned int valM;
    unsigned int dstE;
    unsigned int dstM;
};

wregister getWregister(void);
void clearWregister(void);
bool writebackStage();
void updateWRegister(unsigned int stat, unsigned int icode, unsigned int valE,
                     unsigned int valM, unsigned int dstE, unsigned int dstM);

#endif	/* WRITEBACKSTAGE_H */

