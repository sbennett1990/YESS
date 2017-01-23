/*
 * registers.h
 */

#ifndef REGISTERS_H
#define REGISTERS_H

#define REGSIZE    8     /* number of registers */
#define EAX      0x0
#define ECX      0x1
#define EDX      0x2
#define EBX      0x3
#define ESP      0x4
#define EBP      0x5
#define ESI      0x6
#define EDI      0x7
#define RNONE    0xf     /* i.e. - no register needed */
/* condition codes (CC) */
#define ZF       0x2     /* zero flag     - bit 2 of the CC */
#define SF       0x1     /* sign flag     - bit 1 of the CC */
#define OF       0x0     /* overflow flag - bit 0 of the CC */

void clearCC(void);
void clearRegisters(void);
unsigned int getCC(unsigned int bitNumber);
unsigned int getRegister(int regNum);
void setCC(unsigned int bitNumber, unsigned int value);
void setRegister(int regNum, unsigned int regValue);

#endif	/* REGISTERS_H */

