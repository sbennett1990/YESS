/*
 * File:   decodeStage.h
 */

#ifndef	DECODESTAGE_H
#define	DECODESTAGE_H

#include "forwarding.h"
#include "control.h"

typedef struct {
	unsigned int stat;
	unsigned int icode;
	unsigned int ifun;
	unsigned int rA;
	unsigned int rB;
	unsigned int valC;
	unsigned int valP;
} dregister;

/*
 * +----+---------------------------------------------------------------------+
 * | D  | icode | ifun  |      | rA | rB |   valC   |   valP   |              |
 * +----+---------------------------------------------------------------------+
 */
struct D {  // dregister
	unsigned int stat;
	unsigned int icode;
	unsigned int ifun;
	unsigned int rA;	// Program Register
	unsigned int rB;	// Program Register
	unsigned int valC;
	unsigned int valP;
};

dregister getDregister(void);
void clearDregister(void);
void decodeStage(forwardType, controlType *);
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun,
	unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP);

#endif	/* DECODESTAGE_H */
