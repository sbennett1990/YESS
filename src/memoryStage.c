/*
 * File:   memoryStage.c
 * Author: Alex Savarda
 */

#include "bool.h"
#include "tools.h"
#include "memory.h"
#include "instructions.h"
#include "memoryStage.h"
#include "writebackStage.h"

/*
 * M register holds the input for the memory stage.
 */
static struct mregister M;

static unsigned int select_memory_addr(const struct mregister * const);
static bool mem_write(const struct mregister * const);
static bool mem_read(const struct mregister * const);
static bool stallW(const forwardType * const);
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
 * Clear M register then initialize its icode to NOP and its stat to SAOK.
 */
void
clearMregister()
{
	M.stat = s_okay;
	M.icode = i_nop;
	M.Cnd = 0;
	M.valE = 0;
	M.valA = 0;
	M.dstE = r_none;
	M.dstM = r_none;
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
	unsigned int valM = 0;	/* the data read from memory */
	bool memError = FALSE;

	{
		unsigned int memaddress = select_memory_addr(&M);

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
		stat = s_addr;
	}

	fwd->m_stat = stat;
	fwd->M_icode = M.icode;
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
		updateWRegister(stat, M.icode, M.valE, valM, M.dstE, M.dstM);
	}
}

/*
 * Select memory address to read from or write to.
 *
 * @return The memory address. Default is NOADDRESS.
 */
unsigned int
select_memory_addr(const struct mregister * const mreg)
{
	unsigned int address;

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
mem_read(const struct mregister * const mreg)
{
	switch (mreg->icode.ic) {
	case MRMOVL:
	case POPL:
	case RET:
		return TRUE;

	default:
		return FALSE;
	}
}

/*
 * Should data (valA) be written to memory?
 */
bool
mem_write(const struct mregister * const mreg)
{
	switch (mreg->icode.ic) {
	case RMMOVL:
	case PUSHL:
	case CALL:
		return TRUE;

	default:
		return FALSE;
	}
}

/*
 * Returns true if the W register should be stalled
 *
 * @param status Holds values of statuses forwarded from later stages
 */
bool
stallW(const forwardType * const fwd)
{
	if (fwd->W_stat.s == SADR
	    || fwd->W_stat.s == SINS
	    || fwd->W_stat.s == SHLT) {
		return TRUE;
	}

	return FALSE;
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
