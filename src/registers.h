/*
 * registers.h
 */

#ifndef	REGISTERS_H
#define	REGISTERS_H

#define REGSIZE	8	/* number of registers */

/* Program Registers */
#define EAX		0x0
#define ECX		0x1
#define EDX		0x2
#define EBX		0x3
#define ESP		0x4
#define EBP		0x5
#define ESI		0x6
#define EDI		0x7
#define RNONE		0xf

/* Program Register type */
typedef struct {
	int reg;
} rregister;

/* Condition Codes (CC) */
/*
 * Set with each arithmetic/logical operation (OPL).
 * ZF: was the result 0?
 * SF: was the result < 0?
 * OF: did the result overflow? (2's complement)
 */
#define ZF	0x2	/* zero flag     - bit 2 of the CC */
#define SF	0x1	/* sign flag     - bit 1 of the CC */
#define OF	0x0	/* overflow flag - bit 0 of the CC */


void clearRegisters(void);
unsigned int getRegister(rregister regNum);
void setRegister(rregister regNum, unsigned int regValue);

void clearCC(void);
int getCC(short bitNumber);
void setCC(short bitNumber, unsigned int value);

#endif	/* REGISTERS_H */
