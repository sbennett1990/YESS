/*
 * File:   writebackStage.h
 */

#ifndef	WRITEBACKSTAGE_H
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

int writebackStage();
wregister getWregister(void);
void clearWregister(void);
void updateWRegister(unsigned int stat, unsigned int icode, unsigned int valE,
	unsigned int valM, unsigned int dstE, unsigned int dstM);

#endif	/* WRITEBACKSTAGE_H */
