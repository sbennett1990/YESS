/*
 * dump.c
 */

#include <stdio.h>

#include "dump.h"
#include "bool.h"
#include "registers.h"
#include "memory.h"
#include "fetchStage.h"
#include "decodeStage.h"
#include "executeStage.h"
#include "memoryStage.h"
#include "writebackStage.h"

#include "logger.h"

#define WORDSPERLINE 8
#define LINELENGTH

static void dumpLine(unsigned int *line, short arrlen, int address);
static void buildLine(unsigned int *line, short arrlen, int address);
static int areEqual(unsigned int *, unsigned int *, short);
static void copy(unsigned int *pLine, unsigned int *cLine, short arrlen);

/*
 * Outputs the contents of the YESS little-endian memory WORDSPERLINE
 * four-byte words per line.  A * is displayed at the end of a line if
 * each line in memory after that up to the next line displayed is
 * identical to the * line.
 */
void
dumpMemory(void)
{
	unsigned int prevLine[WORDSPERLINE];
	unsigned int currLine[WORDSPERLINE];

	/* first dump the 0th line */
	int address = 0;
	buildLine(prevLine, WORDSPERLINE, address);
	dumpLine(prevLine, WORDSPERLINE, address);

	int star = 0;
	for (address = WORDSPERLINE; address < MEMSIZE; address += WORDSPERLINE) {
		buildLine(currLine, WORDSPERLINE, address);

		if (areEqual(prevLine, currLine, WORDSPERLINE)) {
			if (!star) {
				printf("*\n");
				star = 1;
			}
		}
		else {
			printf("\n");
			dumpLine(currLine, WORDSPERLINE, address);
			star = 0;
		}

		copy(prevLine, currLine, WORDSPERLINE);
	}

	printf("\n");
}

/*
 * Copies the contents of the cLine array into the pLine array.
 *
 * Parameters:
 *	pLine    array of ints (previous line)
 *	cLine    array of ints (current line)
 *	arrlen   length of both arrays
 *
 * Return pLine - set to values in cLine (current line)
 */
void
copy(unsigned int *pLine, unsigned int *cLine, short arrlen)
{
	int i;
	for (i = 0; i < arrlen; i++) {
		pLine[i] = cLine[i];
	}
}

/*
 * Print the memory address and the contents of the line array.
 *
 * Parameters:
 *	line     array of words read from memory to output
 *	arrlen   length of the line array
 *	address  starting byte address of the line of memory
 */
void
dumpLine(unsigned int *line, short arrlen, int address)
{
	printf("%03x: ", address * 4);

	int i;
	for (i = 0; i < arrlen; i++) {
		printf("%08x ", line[i]);
	}
}

/*
 * Access memory for WORDSPERLINE words and sets the line array to the
 * WORDSPERLINE words from memory.
 *
 * Parameters:
 *	line     array of words read from memory
 *	arrlen   length of the line array
 *	address  starting index of memory access
 *
 * Modifies:
 *	line - array set to values in memory
 */
void
buildLine(unsigned int *line, short arrlen, int address)
{
	bool memError;
	int i;
	for (i = 0; i < arrlen; i++, address++) {
		line[i] = getWord((address * 4), &memError);
		if (memError) {
			log_warn("dumpMemory: couldn't read word at %03x",
			    address);
		}
	}
}

/*
 * Compare the contents of two arrays. Return TRUE if all
 * values are equal.
 *
 * Parameters:
 *	arr1     array of ints
 *	arr2     array of ints
 *	arrlen   length of both arrays
 *
 * Return TRUE if the arrays are identical.
 */
int
areEqual(unsigned int *arr1, unsigned int *arr2, short arrlen)
{
	int i;
	for (i = 0; i < arrlen; i++) {
		if (arr1[i] != arr2[i]) {
			return FALSE;
		}
	}

	return TRUE;
}

/*
 * Print the contents of the YESS program registers.
 */
void
dumpProgramRegisters(void)
{
	printf("%%eax: %08x %%ecx: %08x %%edx: %08x %%ebx: %08x\n",
	    getRegister(r_eax), getRegister(r_ecx), getRegister(r_edx),
	    getRegister(r_ebx));
	printf("%%esp: %08x %%ebp: %08x %%esi: %08x %%edi: %08x\n\n",
	    getRegister(r_esp), getRegister(r_ebp), getRegister(r_esi),
	    getRegister(r_edi));
}

/*
 * Print the contents of the YESS processor registers.
 */
void
dumpProcessorRegisters(void)
{
	struct fregister F = getFregister();
	struct dregister D = getDregister();
	struct eregister E = getEregister();
	struct mregister M = getMregister();
	struct wregister W = getWregister();

	printf("CC - ZF: %01x SF: %01x OF: %01x\n",
	    getCC(ZF), getCC(SF), getCC(OF));
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
