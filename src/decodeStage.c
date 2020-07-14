/*
 * Copyright (c) 2014, 2016, 2020 Scott Bennett <scottb@fastmail.com>
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

#include "bool.h"
#include "tools.h"
#include "registers.h"
#include "instructions.h"
#include "decodeStage.h"
#include "executeStage.h"

/*
 * D register holds the input from the fetch stage.
 */
static struct dregister D;

static rregister getSrcA(const struct dregister *);
static rregister getSrcB(const struct dregister *);
static rregister getDstE(const struct dregister *);
static rregister getDstM(const struct dregister *);
static unsigned int selectFwdA(const struct dregister *, rregister srcA,
    const forwardType *);
static unsigned int forwardB(rregister srcB, const forwardType *);
static bool stallE(void);
static bool bubbleE(const controlType *);

/*
 * Return a copy of the D register
 */
struct dregister
getDregister()
{
	return D;
}

/*
 * Clear D register then initialize its icode to NOP and
 * its stat to SAOK.
 */
void
clearDregister()
{
	rregister rnone = { RNONE };
	D.stat = SAOK;
	D.icode = NOP;
	D.ifun = 0;
	D.rA = rnone;
	D.rB = rnone;
	D.valC = 0;
	D.valP = 0;
}

/*
 * Read up to two operands from the register file, giving values
 * to valA and/or valB.
 *
 * Parameters:
 *  forward	Holds values forwarded from previous stages
 *  *control	Pointer to struct that holds values forwarded from later stages
 */
void
decodeStage(forwardType forward, controlType * control)
{
	rregister srcA = getSrcA(&D);
	rregister srcB = getSrcB(&D);
	rregister dstE = getDstE(&D);
	rregister dstM = getDstM(&D);
	unsigned int valA = selectFwdA(&D, srcA, &forward);
	unsigned int valB = forwardB(srcB, &forward);

	// Update values that need to be forwarded
	control->d_srcA = srcA.reg;
	control->d_srcB = srcB.reg;
	control->D_icode = D.icode;

	// Bubble E?
	if (bubbleE(control)) {
		// Insert a NOP
		rregister rnone = { RNONE };
		updateEregister(SAOK, NOP, 0, 0, 0, 0, rnone, rnone,
		    rnone, rnone);
	}
	else {
		// Update E as normal
		updateEregister(D.stat, D.icode, D.ifun, D.valC, valA, valB,
		    dstE, dstM, srcA, srcB);
	}
}

/*
 * Update the values in the D register
 */
void
updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun,
    rregister rA, rregister rB, unsigned int valC, unsigned int valP)
{
	D.stat = stat;
	D.icode = icode;
	D.ifun = ifun;
	D.rA = rA;
	D.rB = rB;
	D.valC = valC;
	D.valP = valP;
}

/*
 * Retrieve the first register id (register number)
 * based on current icode.
 *
 * @return register id needed
 */
rregister
getSrcA(const struct dregister *dreg)
{
    rregister srcA = { RNONE };

    switch (dreg->icode) {
        case RRMOVL:
        case RMMOVL:
        case OPL:
        case PUSHL:
            srcA = dreg->rA;
            break;

        case POPL:
        case RET:
            srcA.reg = ESP;
            break;

        default:
            srcA.reg = RNONE;
    }

    return srcA;
}

/*
 * Retrieve the second register id (register number)
 * based on the current icode.
 *
 * @return register id needed
 */
rregister
getSrcB(const struct dregister *dreg)
{
	rregister srcB = { RNONE };

	switch (dreg->icode) {
	case OPL:
	case RMMOVL:
	case MRMOVL:
		srcB = dreg->rB;
		break;

	case POPL:
	case RET:
	case CALL:
	case PUSHL:
		srcB.reg = ESP;
		break;

	default:
		srcB.reg = RNONE;
	}

	return srcB;
}

/*
 * Determine the destination register for write port E.
 *
 * @return Destination register id
 */
rregister
getDstE(const struct dregister *dreg)
{
	rregister dstE = { RNONE };

    switch (dreg->icode) {
        case OPL:
        case RRMOVL:
        case IRMOVL:
            dstE = dreg->rB;
            break;

        case POPL:
        case RET:
        case CALL:
        case PUSHL:
            dstE.reg = ESP;
            break;

        default:
            dstE.reg = RNONE;
    }

	return dstE;
}

/*
 * Determine the destination register for write port M.
 *
 * @return Destination register id
 */
rregister
getDstM(const struct dregister *dreg)
{
	rregister dstM = { RNONE };

	if (dreg->icode == MRMOVL || dreg->icode == POPL) {
		dstM = dreg->rA;
	}

	return dstM;
}

/*
 * Merge valP signal into valA and implement forwarding logic
 * for source operand valA.
 *
 * @param srcA    Register id used
 * @param forward Holds values forwarded from previous stages
 * @return Value (valA) to send to E register
 */
unsigned int
selectFwdA(const struct dregister *dreg, rregister srcA,
    const forwardType *forward)
{
    if (dreg->icode == CALL || dreg->icode == JXX) {
        return dreg->valP;
    } else if (srcA.reg == RNONE) {
        return 0;
    } else if (srcA.reg == forward->e_dstE) {
        return forward->e_valE;
    } else if (srcA.reg == forward->M_dstM) {
        return forward->m_valM;
    } else if (srcA.reg == forward->M_dstE) {
        return forward->M_valE;
    } else if (srcA.reg == forward->W_dstM) {
        return forward->W_valM;
    } else if (srcA.reg == forward->W_dstE) {
        return forward->W_valE;
    } else {
        return getRegister(srcA);
    }
}

/*
 * Selects which forwarded value to use for valB
 *
 * @param srcB    Register id used
 * @param forward Holds values forwarded from previous stages
 * @return Value (valB) to send to E register
 */
unsigned int
forwardB(rregister srcB, const forwardType *forward)
{
    if (srcB.reg == RNONE) {
        return 0;
    } else if (srcB.reg == forward->e_dstE) {
        return forward->e_valE;
    } else if (srcB.reg == forward->M_dstM) {
        return forward->m_valM;
    } else if (srcB.reg == forward->M_dstE) {
        return forward->M_valE;
    } else if (srcB.reg == forward->W_dstM) {
        return forward->W_valM;
    } else if (srcB.reg == forward->W_dstE) {
        return forward->W_valE;
    } else {
        return getRegister(srcB);
    }
}

/*
 * Determine if the E register should be stalled. According to HCL,
 * E will never be stalled, therefore it returns false.
 */
bool
stallE()
{
	return FALSE;
}

/*
 * Determine if E needs to be bubbled based on input forwarded
 * by later stages.
 *
 * @param control Holds values from later stages
 * @return True if E should be bubbled, false otherwise
 */
bool
bubbleE(const controlType *control)
{
	bool bubble = FALSE;

	if ((control->E_icode == JXX && !control->e_Cnd) ||
	    ((control->E_icode == MRMOVL || control->E_icode == POPL) &&
	    (control->E_dstM == control->d_srcA || control->E_dstM == control->d_srcB))) {
		bubble = TRUE;
	}

	return bubble;
}
