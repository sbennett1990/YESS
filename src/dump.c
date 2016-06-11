/*
 * dump.c
 */

#include <stdio.h>

#include "bool.h"
#include "dump.h"
#include "forwarding.h"
#include "fetchStage.h"
#include "decodeStage.h"
#include "executeStage.h"
#include "memoryStage.h"
#include "writebackStage.h"
#include "registers.h"
#include "memory.h"

#define WORDSPERLINE 8

static void buildLine(int * line, int address);
static void dumpLine(int * line, int address);
static int isEqual(int * prevLine, int * currLine);
static void copy(int * pLine, int * cLine);

/*
 * Output the contents of the YESS memory (little-endian) in four-byte words per
 * line. A * is displayed at the end of a line if each line after that is
 * identical to the * line (up to the next line displayed).
 */
void dumpMemory(void) {
    int star = 0;
    int address = 0;
    int prevLine[WORDSPERLINE];
    int currLine[WORDSPERLINE];

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
 * Output the contents of the YESS program registers to standard out.
 */
void dumpProgramRegisters(void) {
    printf("%%eax: %08x %%ecx: %08x %%edx: %08x %%ebx: %08x\n",
           getRegister(EAX), getRegister(ECX), getRegister(EDX),
           getRegister(EBX));
    printf("%%esp: %08x %%ebp: %08x %%esi: %08x %%edi: %08x\n\n",
           getRegister(ESP), getRegister(EBP), getRegister(ESI),
           getRegister(EDI));
}

/*
 * Output the contents of the YESS processor registers to standard out.
 */
void dumpProcessorRegisters(void) {
    fregister F = getFregister();
    dregister D = getDregister();
    eregister E = getEregister();
    mregister M = getMregister();
    wregister W = getWregister();

    printf("CC - ZF: %01x SF: %01x OF: %01x\n", getCC(ZF), getCC(SF), getCC(OF));
    printf("F - predPC: %08x\n", F.predPC);
    printf("D - stat: %01x icode: %01x ifun: %01x rA: %01x rB: %01x valC: %08x  valP: %08x\n",
           D.stat, D.icode, D.ifun, D.rA, D.rB, D.valC, D.valP);
    printf("E - stat: %01x icode: %01x  ifun: %01x  valC: %08x valA: %08x valB: %08x\n",
           E.stat, E.icode, E.ifun, E.valC, E.valA, E.valB);
    printf("    dstE: %01x dstM: %01x srcA: %01x srcB: %01x\n",
           E.dstE, E.dstM, E.srcA, E.srcB);
    printf("M - stat: %01x icode: %01x Cnd: %01x valE: %08x valA: %08x dstE: %01x dstM: %01x\n",
           M.stat, M.icode, M.Cnd, M.valE, M.valA, M.dstE, M.dstM);
    printf("W - stat: %01x icode: %01x valE: %08x valM: %08x dstE: %01x dstM: %01x\n\n",
           W.stat, W.icode, W.valE, W.valM, W.dstE, W.dstM);
}

/*
 * Access memory for WORDSPERLINE words and set the line array to the
 * WORDSPERLINE words from memory.
 *
 * Parameters:
 *     line       pointer to an array of ints
 *     address    starting index to access memory
 *
 * Modifies:
 *     line - array initialized to values in memory
 */
void buildLine(int * line, int address) {
    bool memError;

    for (int i = 0; i < WORDSPERLINE; i++, address++) {
        line[i] = getWord((address * 4), &memError);
    }
}

/*
 * Output the starting address (to standard out) and the contents of the line
 * array.
 *
 * Parameters:
 *     line       pointer to an array of ints
 *     address    row header
 */
void dumpLine(int * line, int address) {
    printf("%03x: ", (address * 4));

    for (int i = 0; i < WORDSPERLINE; i++) {
        printf("%08x ", line[i]);
    }
}

/*
 * Compare the contents of prevLine and currLine. Return TRUE if the are equal.
 *
 * Parameters:
 *     prevLine    pointer to an array of ints
 *     currLine    pointer to an array of ints
 *
 * Return TRUE if prevLine and currLine are identical
 */
int isEqual(int * prevLine, int * currLine) {

    for (int i = 0; i < WORDSPERLINE; i++) {
        if (prevLine[i] != currLine[i]) {
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * Copy the contents of the cLine array into the pLine array.
 *
 * Parameters:
 *     pLine    set to values in cLine (previous line)
 *     cLine    pointer to an array of ints (current line)
 */
void copy(int * pLine, int * cLine) {

    for (int i = 0; i < WORDSPERLINE; i++) {
        pLine[i] = cLine[i];
    }
}
