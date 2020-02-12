/*
 * File:   decodeStage.c
 * Author: Scott Bennett
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
static dregister D;

static unsigned int getSrcA(dregister *);
static unsigned int getSrcB(dregister);
static unsigned int getDstE(dregister);
static unsigned int getDstM(dregister);
static unsigned int selectFwdA(dregister, unsigned int srcA, forwardType forward);
static unsigned int forwardB(unsigned int srcB, forwardType forward);
static bool stallE(void);
static bool bubbleE(controlType control);

/*
 * Return a copy of the D register
 */
dregister getDregister() {
    return D;
}

/*
 * Clear D register then initialize its icode to NOP and
 * its stat to SAOK.
 */
void clearDregister() {
    clearBuffer((char *) &D, sizeof(D));
    D.stat = SAOK;
    D.icode = NOP;
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
    unsigned int srcA = getSrcA(&D);
    unsigned int srcB = getSrcB(D);
    unsigned int dstE = getDstE(D);
    unsigned int dstM = getDstM(D);
    unsigned int valA = selectFwdA(D, srcA, forward);
    unsigned int valB = forwardB(srcB, forward);

    // Update values that need to be forwarded
    control->d_srcA = srcA;
    control->d_srcB = srcB;
    control->D_icode = D.icode;

    // Bubble E?
    if (bubbleE(*control)) {
        // Insert a NOP
        updateEregister(SAOK, NOP, 0, 0, 0, 0, RNONE, RNONE, RNONE, RNONE);
    }
    else {
        // Update E as normal
        updateEregister(D.stat, D.icode, D.ifun, D.valC, valA, valB, dstE, dstM, srcA,
                        srcB);
    }
}

/*
 * Update the values in the D register
 */
void
updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun,
    unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP)
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
unsigned int getSrcA(const dregister *d) {
    unsigned int srcA = RNONE;

    switch (d->icode) {
        case RRMOVL:
        case RMMOVL:
        case OPL:
        case PUSHL:
            srcA = d->rA;
            break;

        case POPL:
        case RET:
            srcA = ESP;
            break;

        default:
            srcA = RNONE;
    }

    return srcA;
}

/*
 * Retrieve the second register id (register number)
 * based on the current icode.
 *
 * @return register id needed
 */
unsigned int getSrcB(dregister d) {
    unsigned int srcB = RNONE;

    switch (d.icode) {
        case OPL:
        case RMMOVL:
        case MRMOVL:
            srcB = d.rB;
            break;

        case POPL:
        case RET:
        case CALL:
        case PUSHL:
            srcB = ESP;
            break;

        default:
            srcB = RNONE;
    }

    return srcB;
}

/*
 * Determine the destination register for write port E.
 *
 * @return Destination register id
 */
unsigned int getDstE(dregister d) {
    unsigned int dstE = RNONE;

    switch (d.icode) {
        case OPL:
        case RRMOVL:
        case IRMOVL:
            dstE = d.rB;
            break;

        case POPL:
        case RET:
        case CALL:
        case PUSHL:
            dstE = ESP;
            break;

        default:
            dstE = RNONE;
    }

    return dstE;
}

/*
 * Determine the destination register for write port M.
 *
 * @return Destination register id
 */
unsigned int getDstM(dregister d) {
    if (d.icode == MRMOVL || d.icode == POPL) {
        return d.rA;
    } else {
        return RNONE;
    }
}

/*
 * Merge valP signal into valA and implement forwarding logic
 * for source operand valA.
 *
 * @param srcA    Register id used
 * @param forward Holds values forwarded from previous stages
 * @return Value (valA) to send to E register
 */
unsigned int selectFwdA(dregister d, unsigned int srcA, forwardType forward) {
    if (d.icode == CALL || d.icode == JXX) {
        return d.valP;
    } else if (srcA == RNONE) {
        return 0;
    } else if (srcA == forward.e_dstE) {
        return forward.e_valE;
    } else if (srcA == forward.M_dstM) {
        return forward.m_valM;
    } else if (srcA == forward.M_dstE) {
        return forward.M_valE;
    } else if (srcA == forward.W_dstM) {
        return forward.W_valM;
    } else if (srcA == forward.W_dstE) {
        return forward.W_valE;
    } else {
        return getRegister(srcA);
    }
}

/*
 *  Selects which forwarded value to use for valB
 *
 * @param srcB    Register id used
 * @param forward Holds values forwarded from previous stages
 * @return Value (valB) to send to E register
 */
unsigned int forwardB(unsigned int srcB, forwardType forward) {
    if (srcB == RNONE) {
        return 0;
    } else if (srcB == forward.e_dstE) {
        return forward.e_valE;
    } else if (srcB == forward.M_dstM) {
        return forward.m_valM;
    } else if (srcB == forward.M_dstE) {
        return forward.M_valE;
    } else if (srcB == forward.W_dstM) {
        return forward.W_valM;
    } else if (srcB == forward.W_dstE) {
        return forward.W_valE;
    } else {
        return getRegister(srcB);
    }
}

/*
 * Determine if the E register should be stalled. According to HCL,
 * E will never be stalled, therefore it returns false.
 */
bool stallE() {
    return FALSE;
}

/*
 * Determine if E needs to be bubbled based on input forwarded
 * by later stages.
 *
 * @param control Holds values from later stages
 * @return True if E should be bubbled, false otherwise
 */
bool bubbleE(controlType control) {
    bool bubble = FALSE;

    if ((control.E_icode == JXX && !control.e_Cnd) ||
        (control.E_icode == MRMOVL || control.E_icode == POPL) &&
        (control.E_dstM == control.d_srcA || control.E_dstM == control.d_srcB)) {
        bubble = TRUE;
    }

    return bubble;
}
