/*
 * dump.c
 */

#include <stdio.h>

#include "bool.h"
#include "dump.h"
#include "fetchStage.h"
#include "decodeStage.h"
#include "executeStage.h"
#include "memoryStage.h"
#include "writebackStage.h"
#include "registers.h"
#include "memory.h"

#define WORDSPERLINE 8
#define LINELENGTH

static void dumpLine(int line[WORDSPERLINE], int address);
static void buildLine(int line[WORDSPERLINE], int address);
static int isEqual(int prevLine[WORDSPERLINE], int currLine[WORDSPERLINE]);
static void copy(int *, int *);

/*
 * Outputs the contents of the YESS little-endian memory WORDSPERLINE
 * four-byte words per line.  A * is displayed at the end of a line if
 * each line in memory after that up to the next line displayed is
 * identical to the * line.
 */
void dumpMemory(void) {
    int address = 0;
    int prevLine[WORDSPERLINE];
    int currLine[WORDSPERLINE];
    int star = 0;

    buildLine(prevLine, address);
    dumpLine(prevLine, address);

    for (address = WORDSPERLINE; address < MEMSIZE; address += WORDSPERLINE) {
        buildLine(currLine, address);

        if (isEqual(prevLine, currLine)) {
            if (!star) {
                printf("*\n");
                star = 1;
            }
        } else {
            printf("\n");
            dumpLine(currLine, address);
            star = 0;
        }

        copy(prevLine, currLine);
    }

    printf("\n");
}

/*
 * Copies the contents of the cLine array into the pLine array.
 *
 * Parameters:
 *     cLine    array of ints (current line)
 *
 * Return pLine - initialized to values in cLine (previous line)
 */
void copy(int pLine[WORDSPERLINE], int cLine[WORDSPERLINE]) {
    int i;

    for (i = 0; i < WORDSPERLINE; i++) {
        pLine[i] = cLine[i];
    }
}

/*
 * Output the starting address in the variable address and the contents of the
 * line array.
 *
 * Parameters:
 *      line         array of ints to output
 *      address   row header
 */
void dumpLine(int line[WORDSPERLINE], int address) {
    int i;
    printf("%03x: ", address * 4);

    for (i = 0; i < WORDSPERLINE; i++) {
        printf("%08x ", line[i]);
    }
}

/*
 * Access memory for WORDSPERLINE words and sets the line array to the
 * WORDSPERLINE words from memory.
 *
 * Parameters:
 *      line         array of ints
 *      address   starting index to access memory
 *
 * Modifies:
 *      line - array initialized to values in memory
 */
void buildLine(int line[WORDSPERLINE], int address) {
    int i;
    bool memError;
    for (i = 0; i < WORDSPERLINE; i++, address++) {
        line[i] = getWord((address * 4), &memError);
    }
}

/*
 * Compare the contents of prevLine and currLine. Return TRUE if the are equal.
 *
 * Parameters:
 *      prevLine   array of ints
 *      currLine    array of ints
 *
 * Return TRUE if prevLine and currLine are identical
 */
int isEqual(int prevLine[WORDSPERLINE], int currLine[WORDSPERLINE]) {
    int i;

    for (i = 0; i < WORDSPERLINE; i++) {
        if (prevLine[i] != currLine[i]) {
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * Output the contents of the YESS program registers to standard out.
 */
void dumpProgramRegisters(void) {
	rregister eax = { EAX };
	rregister ecx = { ECX };
	rregister edx = { EDX };
	rregister ebx = { EBX };
	rregister esp = { ESP };
	rregister ebp = { EBP };
	rregister esi = { ESI };
	rregister edi = { EDI };
    printf("%%eax: %08x %%ecx: %08x %%edx: %08x %%ebx: %08x\n",
           getRegister(eax), getRegister(ecx), getRegister(edx),
           getRegister(ebx));
    printf("%%esp: %08x %%ebp: %08x %%esi: %08x %%edi: %08x\n\n",
           getRegister(esp), getRegister(ebp), getRegister(esi),
           getRegister(edi));
}

/*
 * Output the contents of the YESS processor registers to standard out.
 */
void dumpProcessorRegisters(void) {
    struct fregister F = getFregister();
    struct dregister D = getDregister();
    struct eregister E = getEregister();
    struct mregister M = getMregister();
    struct wregister W = getWregister();

    printf("CC - ZF: %01x SF: %01x OF: %01x\n", getCC(ZF), getCC(SF), getCC(OF));
    printf("F - predPC: %08x\n", F.predPC);
    printf("D - stat: %01x icode: %01x ifun: %01x rA: %01x rB: %01x valC: %08x  valP: %08x\n",
           D.stat.s, D.icode.ic, D.ifun, D.rA.reg, D.rB.reg, D.valC, D.valP);
    printf("E - stat: %01x icode: %01x ifun: %01x  valC: %08x valA: %08x valB: %08x\n",
           E.stat.s, E.icode.ic, E.ifun, E.valC, E.valA, E.valB);
    printf("    dstE: %01x dstM: %01x srcA: %01x srcB: %01x\n",
           E.dstE.reg, E.dstM.reg, E.srcA.reg, E.srcB.reg);
    printf("M - stat: %01x icode: %01x Cnd: %01x valE: %08x valA: %08x dstE: %01x dstM: %01x\n",
           M.stat.s, M.icode.ic, M.Cnd, M.valE, M.valA, M.dstE.reg, M.dstM.reg);
    printf("W - stat: %01x icode: %01x valE: %08x valM: %08x dstE: %01x dstM: %01x\n\n",
           W.stat.s, W.icode.ic, W.valE, W.valM, W.dstE.reg, W.dstM.reg);
}
