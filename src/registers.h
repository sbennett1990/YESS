/*
 * registers.h
 */

#ifndef	REGISTERS_H
#define	REGISTERS_H

#define REGSIZE	8	/* total number of program registers */

/* Program Registers */
#define EAX	0x0	/* accumulator */
#define ECX	0x1	/* counter */
#define EDX	0x2	/* data register */
#define EBX	0x3	/* base register */
#define ESP	0x4	/* stack pointer */
#define EBP	0x5	/* stack base pointer */
#define ESI	0x6	/* source index */
#define EDI	0x7	/* destination index */
#define RNONE	0xf	/* "no register" */

/* Program Register type */
typedef struct {
	int reg;
} rregister;

/* Register typedefs */
#define r_eax	&(rregister){ EAX }
#define r_eax	&(rregister){ EAX }
#define r_ecx	&(rregister){ ECX }
#define r_edx	&(rregister){ EDX }
#define r_ebx	&(rregister){ EBX }
#define r_esp	&(rregister){ ESP }
#define r_ebp	&(rregister){ EBP }
#define r_esi	&(rregister){ ESI }
#define r_edi	&(rregister){ EDI }
#define r_none	&(rregister){ RNONE }


/* Condition Codes (CC)
 * Set with each arithmetic/logical operation (OPL).
 *
 * ZF: was the result 0?
 * SF: was the result < 0?
 * OF: did the result overflow? (2's complement)
 */
#define ZF	0x2	/* zero flag     - bit 2 of the CC */
#define SF	0x1	/* sign flag     - bit 1 of the CC */
#define OF	0x0	/* overflow flag - bit 0 of the CC */

/*
 * Compare two register types to determine if they are the same.
 * Note that this does not compare the *data* in either
 * register.
 *
 * Parameters:
 *	r1     First program register
 *	r2     Second program register
 */
#define reg_cmp(r1, r2)	((r1).reg == (r2).reg)

/*
 * Determine if the given register type is a specific register.
 *
 * Parameters:
 *	reg    Register type
 *	r      Program register
 */
#define reg_is(reg, r)	((reg).reg == r)

void clearRegisters(void);
unsigned int getRegister(rregister regNum);
void setRegister(rregister regNum, unsigned int regValue);

void clearCC(void);
int getCC(short bitNumber);
void setCC(short bitNumber, unsigned int value);

#endif	/* REGISTERS_H */
