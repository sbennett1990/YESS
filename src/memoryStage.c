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

static unsigned int mem_addr(const struct mregister *);
static bool mem_write(const struct mregister *);
static bool mem_read(const struct mregister *);
static bool stallW(statusType status);
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
	M.stat = SAOK;
	M.icode = NOP;
	M.Cnd = 0;
	M.valE = 0;
	M.valA = 0;
	M.dstE = RNONE;
	M.dstM = RNONE;
}

/*
 * May write data to memory or read data from memory.
 * Updates the W register.
 *
 * Parameters:
 *  *forward	Holds values forwarded to previous stages
 *  *status	Holds values forwarded to previous stages
 *  *control	Holds values forwarded to previous stages
 */
void
memoryStage(forwardType * forward, statusType * status, controlType * control)
{
	unsigned int address = mem_addr(&M);
	unsigned int stat = M.stat;
	unsigned int valM = NOADDRESS;
	bool memError = FALSE;

	// read data from memory?
	if (mem_read(&M)) {
		valM = getWord(address, &memError);
	}

	// write data to memory?
	if (mem_write(&M)) {
		putWord(address, M.valA, &memError);
	}

	if (memError) {
		stat = SADR;
	}

	forward->M_dstM = M.dstM;
	forward->M_dstE = M.dstE;
	forward->m_valM = valM;
	forward->M_valE = M.valE;
	forward->M_Cnd = M.Cnd;
	forward->M_valA = M.valA;
	forward->M_icode = M.icode;
	status->m_stat = stat;
	control->M_icode = M.icode;

	if (!stallW(*status)) {
		/*
		 * If stall is true, do nothing to keep current values in the
		 * Writeback Stage.
		 */
		rregister dstE = { M.dstE };
		rregister dstM = { M.dstM };
		updateWRegister(stat, M.icode, M.valE, valM, dstE, dstM);
	}
}

/*
 * Update the values in the M register.
 */
void
updateMRegister(unsigned int stat, unsigned int icode, unsigned int Cnd,
    unsigned int valE, unsigned int valA, rregister dstE, rregister dstM)
{
	M.stat = stat;
	M.icode = icode;
	M.Cnd = Cnd;
	M.valE = valE;
	M.valA = valA;
	M.dstE = dstE.reg;
	M.dstM = dstM.reg;
}

/*
 * Select memory address to read from or write to.
 *
 * @return The memory address. Default is NOADDRESS.
 */
unsigned int
mem_addr(const struct mregister *mreg)
{
	unsigned int address = NOADDRESS;

	switch (mreg->icode) {
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
 * Set write control signal
 */
bool
mem_write(const struct mregister *mreg)
{
    bool write = FALSE;

    switch (mreg->icode) {
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
 * Set read control signal
 */
bool
mem_read(const struct mregister *mreg)
{
    bool read = FALSE;

    switch (mreg->icode) {
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
 * Returns true if the W register should be stalled
 *
 * @param status Holds values of statuses forwarded from later stages
 */
bool
stallW(statusType status)
{
	bool stall = FALSE;

	if (status.W_stat == SADR
	    || status.W_stat == SINS
	    || status.W_stat == SHLT) {
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
