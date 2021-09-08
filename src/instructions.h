/*
 * Copyright (c) 2021 Scott Bennett <scottb@fastmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef	INSTRUCTIONS_H
#define	INSTRUCTIONS_H

#include <stdint.h>

#define INSTR_COUNT	16	/* Size of the instruction set */

/* Instruction Code type */
typedef struct {
	uint8_t	ic;
} icode_t;

/*
 * Instruction Codes (icode's)
 */
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

extern const icode_t i_halt;
extern const icode_t i_nop;
extern const icode_t i_dump;

/*
 * Instruction Functions (ifun's)
 */

/* Arithmetic and Logical Instructions */
#define ADDL	0x0
#define SUBL	0x1
#define ANDL	0x2
#define XORL	0x3

/* Conditional Move Instructions (Register -> Register) */
#define RRMOVLF	0x0	/* Move Unconditionally */
#define CMOVLE	0x1	/* Move When Less or Equal */
#define CMOVL	0x2	/* Move When Less */
#define CMOVE	0x3	/* Move When Equal */
#define CMOVNE	0x4	/* Move When Not Equal */
#define CMOVGE	0x5	/* Move When Greater or Equal */
#define CMOVG	0x6	/* Move When Greater */

/* Jump Instructions */
#define JMP	0x0	/* Jump Unconditionally */
#define JLE	0x1	/* Jump When Less or Equal */
#define JL	0x2	/* Jump When Less */
#define JE	0x3	/* Jump When Equal */
#define JNE	0x4	/* Jump When Not Equal */
#define JGE	0x5	/* Jump When Greater or Equal */
#define JG	0x6	/* Jump When Greater */


int icode_is(icode_t it, uint8_t ic);


/* Status Code type */
typedef struct {
	uint8_t	s;
} stat_t;

/*
 * Status Codes
 */
#define SAOK	1	/* Normal operation */
#define SHLT	2	/* Halt instruction encountered */
#define SADR	3	/* Bad address encountered */
#define SINS	4	/* Invalid instruction encountered */

extern const stat_t s_okay;
extern const stat_t s_halt;
extern const stat_t s_addr;
extern const stat_t s_inst;

#endif	/* INSTRUCTIONS_H */
