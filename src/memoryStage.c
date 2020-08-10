/*
 * File:   memoryStage.c
 * Author: Alex Savarda
 */

#include "bool.h"
#include "tools.h"
#include "memory.h"
#include "instructions.h"
#include "registers.h"
#include "memoryStage.h"
#include "writebackStage.h"

/*
 * M register holds the input for the memory stage.
 */
static struct mregister M;

static unsigned int memory_addr(const struct mregister *);
static bool mem_write(const struct mregister *);
static bool mem_read(const struct mregister *);
static bool stallW(const forwardType *);
static bool bubbleW(void);

/*
 * Return a copy of the M register
 */
struct mregister
getMregister()
{
	return M;
}

/*
 * Clear M register then initialize its icode to NOP and
 * its stat to SAOK.
 */
void
clearMregister()
{
	stat_t okay = { SAOK };
	icode_t nop = { NOP };
	rregister rnone = { RNONE };

	M.stat = okay;
	M.icode = nop;
	M.Cnd = 0;
	M.valE = 0;
	M.valA = 0;
	M.dstE = rnone;
	M.dstM = rnone;
}

/*
 * May write data to memory or read data from memory.
 * Updates the W register.
 *
 * Parameters:
 *	*fwd    Holds values forwarded to previous stages
 */
void
memoryStage(forwardType *fwd)
{
	stat_t stat = M.stat;
	unsigned int valM = NOADDRESS;	/* value read from memory */
	bool memError = FALSE;

	{
		unsigned int memaddress = memory_addr(&M);
		// read data from memory?
		if (mem_read(&M)) {
			valM = getWord(memaddress, &memError);
		}
		// write data to memory?
		if (mem_write(&M)) {
			putWord(memaddress, M.valA, &memError);
		}
	}

	if (memError) {
		stat.s = SADR;
	}

	fwd->m_stat = stat;
	fwd->M_icode = M.icode.ic;
	fwd->M_Cnd = M.Cnd;
	fwd->M_dstM = M.dstM;
	fwd->m_valM = valM;
	fwd->M_dstE = M.dstE;
	fwd->M_valE = M.valE;
	fwd->M_valA = M.valA;

	if (!stallW(fwd)) {
		/*
		 * If stall is true, do nothing to keep current values in the
		 * Writeback Stage.
		 */
		updateWRegister(stat, M.icode.ic, M.valE, valM, M.dstE, M.dstM);
	}
}

/*
 * Update the values in the M register.
 */
void
updateMRegister(stat_t stat, icode_t icode, unsigned int Cnd,
    unsigned int valE, unsigned int valA, rregister dstE, rregister dstM)
{
	M.stat = stat;
	M.icode = icode;
	M.Cnd = Cnd;
	M.valE = valE;
	M.valA = valA;
	M.dstE = dstE;
	M.dstM = dstM;
}

/*
 * Select memory address to read from or write to.
 *
 * @return The memory address. Default is NOADDRESS.
 */
unsigned int
memory_addr(const struct mregister *mreg)
{
	unsigned int address = NOADDRESS;

	switch (mreg->icode.ic) {
	case RMMOVL:
	case PUSHL:
	case CALL:
	case MRMOVL:
		address = mreg->valE;
		break;

	case POPL:
	case RET:
		address = mreg->valA;
		break;

	default:
		address = NOADDRESS;
	}

	return address;
}

/*
 * Should data be read from memory?
 */
bool
mem_read(const struct mregister *mreg)
{
	bool read = FALSE;

	switch (mreg->icode.ic) {
	case MRMOVL:
	case POPL:
	case RET:
		read = TRUE;
		break;

	default:
		read = FALSE;
	}

	return read;
}

/*
 * Should data (valA) be written to memory?
 */
bool
mem_write(const struct mregister *mreg)
{
	bool write = FALSE;

	switch (mreg->icode.ic) {
	case RMMOVL:
	case PUSHL:
	case CALL:
		write = TRUE;
		break;

	default:
		write = FALSE;
	}

	return write;
}

/*
 * Returns true if the W register should be stalled
 *
 * @param status Holds values of statuses forwarded from later stages
 */
bool
stallW(const forwardType *fwd)
{
	bool stall = FALSE;

	if (fwd->W_stat.s == SADR
	    || fwd->W_stat.s == SINS
	    || fwd->W_stat.s == SHLT) {
		stall = TRUE;
	}

	return stall;
}

/*
 * Determine if the W register should be bubbled. According to HCL,
 * W will never be bubbled, therefore it returns false.
 */
bool
bubbleW()
{
	return FALSE;
}
