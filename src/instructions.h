/*
 * instructions.h
 */

#ifndef	INSTRUCTIONS_H
#define	INSTRUCTIONS_H

#include <stdint.h>

#define INSTR_COUNT	16	/* Possible size of the instruction set */

// Instruction Codes (icode)
#define HALT	0x0
#define NOP	0x1
#define RRMOVL	0x2	/* Move Operation: Register  -> Register */
#define IRMOVL	0x3	/* Move Operation: Immediate -> Register */
#define RMMOVL	0x4	/* Move Operation: Register  -> Memory */
#define MRMOVL	0x5	/* Move Operation: Memory    -> Register */
#define OPL	0x6	/* Arithmetic and Logical Operations */
#define JXX	0x7	/* Jump Operations */
#define CALL	0x8
#define RET	0x9
#define PUSHL	0xA
#define POPL	0xB
#define DUMP	0xC	/* Output parts of the Y86 machine state */

// Instruction Functions (ifun)

/* Arithmetic and Logical Instructions */
#define ADDL	0x0
#define SUBL	0x1
#define ANDL	0x2
#define XORL	0x3

/* Conditional Move Instructions */
#define RRMOVLF	0x0
#define CMOVLE	0x1
#define CMOVL	0x2
#define CMOVE	0x3
#define CMOVNE	0x4
#define CMOVGE	0x5
#define CMOVG	0x6

/* Jump Instructions */
#define JMP	0x0	/* Jump Unconditionally */
#define JLE	0x1	/* Jump When Less or Equal */
#define JL	0x2	/* Jump When Less */
#define JE	0x3	/* Jump When Equal */
#define JNE	0x4	/* Jump When Not Equal */
#define JGE	0x5	/* Jump When Greater or Equal */
#define JG	0x6	/* Jump When Greater */

/* Instruction Code type */
typedef struct {
	uint8_t	ic;
} icode_t;


/* Status Codes */
#define SAOK	1	/* Normal operation */
#define SHLT	2	/* Halt instruction encountered */
#define SADR	3	/* Bad address encountered */
#define SINS	4	/* Invalid instruction encountered */

/* Status Code type */
typedef struct {
	uint8_t	s;
} stat_t;

#endif	/* INSTRUCTIONS_H */
