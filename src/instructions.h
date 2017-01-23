/*
 * File:   instructions.h
 * Author: Scott Bennett
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

// icodes
#define HALT    0x0
#define NOP     0x1
#define RRMOVL  0x2
#define IRMOVL  0x3
#define RMMOVL  0x4
#define MRMOVL  0x5
#define OPL     0x6
#define JXX     0x7
#define CALL    0x8
#define RET     0x9
#define PUSHL   0xA
#define POPL    0xB
#define DUMP    0xC

// ifuns
#define ADDL    0x0
#define SUBL    0x1
#define ANDL    0x2
#define XORL    0x3

#define RRMOVLF 0x0
#define CMOVLE  0x1
#define CMOVL   0x2
#define CMOVE   0x3
#define CMOVNE  0x4
#define CMOVGE  0x5
#define CMOVG   0x6

#define JMP     0x0
#define JLE     0x1
#define JL      0x2
#define JE      0x3
#define JNE     0x4
#define JGE     0x5
#define JG      0x6

// Status codes
#define SAOK  1
#define SHLT  2
#define SADR  3
#define SINS  4

#endif /* INSTRUCTIONS_H */

