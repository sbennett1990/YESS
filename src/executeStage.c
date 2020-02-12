/*
 * File:   executeStage.c
 * Author: Alex Savarda
 */

#include "bool.h"
#include "tools.h"
#include "registers.h"
#include "instructions.h"
#include "executeStage.h"
#include "memoryStage.h"

/*
 * E register holds the input from the decode stage.
 */
static eregister E;

static unsigned int (*funcPtr[INSTR_COUNT])(void);
static unsigned int performZero(void);
static unsigned int performDUMP(void);
static unsigned int performIrmovl(void);
static unsigned int performOpl(void);
static unsigned int performRrmovl(void);  // also is Cmovxx
static unsigned int performMrmovl(void);
static unsigned int performRmmovl(void);
static unsigned int performPopl(void);
static unsigned int performPushl(void);
static unsigned int performJXX(void);
static unsigned int performCall(void);
static unsigned int performRet(void);

static void computeCC(int result, int a, int b);	// XXX: what was this?
static int computeCnd(eregister);

static bool M_stall(void);
static bool M_bubble(statusType status);

static bool changeCC;

/*
 * Return a copy of the E register
 */
eregister
getEregister()
{
    return E;
}

/*
 * Clear E register then initialize its icode to NOP and its stat to SAOK.
 */
void
clearEregister()
{
    clearBuffer((char *) &E, sizeof(E));
    E.icode = NOP;
    E.stat = SAOK;
}

/*
 * Perform the operation based on the instruction.
 * Compute e_Cnd and dstE and update the values in the M
 * register.
 *
 * @param *forward  Pointer to a forwardType
 * @param status    Holds status values from later stages
 * @param *control  Pointer to struct that holds values forwarded from
 *                    later stages
 */
void
executeStage(forwardType * forward, statusType status, controlType * control)
{
    bool m_bubble = M_bubble(status);
    changeCC = TRUE;

    // If either m_stat or W_stat are SINS, SADR, or SHLT, then do not modify CC's
    if (status.m_stat == SINS || status.m_stat == SADR || status.m_stat == SHLT ||
        status.W_stat == SINS || status.W_stat == SADR || status.W_stat == SHLT) {
        changeCC = FALSE;
        m_bubble = TRUE;
    }

    // Execute the instruction and compute Cnd
    unsigned int valE = funcPtr[E.icode]();
    int e_Cnd = computeCnd(E);

    if ((E.icode == RRMOVL) && !e_Cnd) {
        E.dstE = RNONE;
    }

    forward->e_dstE = E.dstE;
    forward->e_valE = valE;

    control->E_icode = E.icode;
    control->e_Cnd = e_Cnd;
    control->E_dstM = E.dstM;

    // Bubble M?
    if (m_bubble) {}
        // Insert a NOP
        updateMRegister(SAOK, NOP, 0, 0, 0, RNONE, RNONE);
    }
    else {
        // Update M register as normal
        updateMRegister(E.stat, E.icode, e_Cnd, valE, E.valA, E.dstE, E.dstM);
    }
}

/*
 * Update the values in the E register.
 */
void
updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun,
    unsigned int valC, unsigned int valA, unsigned int valB, unsigned int dstE,
	unsigned int dstM, unsigned int srcA, unsigned int srcB)
{
    E.stat = stat;
    E.icode = icode;
    E.ifun = ifun;
    E.valC = valC;
    E.valA = valA;
    E.valB = valB;
    E.dstE = dstE;
    E.dstM = dstM;
    E.srcA = srcA;
    E.srcB = srcB;
}

/*
 * Initialize the array of function pointers.
 * This should only be called in main, so that it's initialized once.
 */
void
initFuncPtrArray()
{
    // First initialize array to 0's
    int i;
    for (i = 0; i < INSTR_COUNT; i++) {
        funcPtr[i] = performZero;
    }

    funcPtr[HALT] = performZero;
    funcPtr[NOP] = performZero;
    funcPtr[RRMOVL] = performRrmovl;
    funcPtr[IRMOVL] = performIrmovl;
    funcPtr[RMMOVL] = performRmmovl;
    funcPtr[MRMOVL] = performMrmovl;
    funcPtr[OPL] = performOpl;
    funcPtr[JXX] = performJXX;
    funcPtr[CALL] = performCall;
    funcPtr[RET] = performRet;
    funcPtr[PUSHL] = performPushl;
    funcPtr[POPL] = performPopl;
    funcPtr[DUMP] = performDUMP;
}

/**
 * Calculate the e_Cnd based on CC and ifun.
 * e_Cnd is 0 for every opcode except RRMOVL
 * and JXX.
 *
 * @return Computed value of e_Cnd
 */
int computeCnd(eregister ereg) {
    int e_Cnd = 0;
    int sf = getCC(SF);
    int zf = getCC(ZF);
    int of = getCC(OF);

    if (ereg.icode == RRMOVL || ereg.icode == JXX) {
        switch (ereg.ifun) {
            case RRMOVLF:
                e_Cnd = 1;
                break;

            case CMOVLE:
                if ((sf ^ of) | zf) {
                    e_Cnd = 1;
                }

                break;

            case CMOVL:
                if (sf ^ of) {
                    e_Cnd = 1;
                }

                break;

            case CMOVE:
                if (zf) {
                    e_Cnd = 1;
                }

                break;

            case CMOVNE:
                if (!zf) {
                    e_Cnd = 1;
                }

                break;

            case CMOVGE:
                if (!(sf ^ of)) {
                    e_Cnd = 1;
                }

                break;

            case CMOVG:
                if (!(sf ^ of) & !zf) {
                    e_Cnd = 1;
                }

                break;

            default:
                e_Cnd = 0;
        }
    }

    return e_Cnd;
}


/**
 * Just returns 0 for HALT and NOP instructions.
 *
 * @return 0
 */
unsigned int performZero() {
    return 0;
}

/**
 * Compute memory location and execute the register
 * to memory move instruction.
 *
 * @return The memory location
 */
unsigned int performRmmovl() {
    return E.valB + E.valC;
}

/**
 * Compute memory location and execute the memory
 * to register move instruction.
 *
 * @return The memory location
 */
unsigned int performMrmovl() {
    return E.valB + E.valC;
}

/**
 * Performs rrmovl and cmovxx instructions.
 *
 * @return The value from rA
 */
unsigned int performRrmovl() {
    return E.valA;
}

/**
 * Perform dump instruction.
 *
 * @return Constant word, part of the instruction
 */
unsigned int performDUMP() {
    return E.valC;
}

/**
 * Perform the immediate to register move
 * instruction.
 *
 * @return Constant word, part of the instruction
 */
unsigned int performIrmovl() {
    return E.valC;
}

/**
 * Perform either an ADD, SUB, AND, or XOR operation
 * and set the CC accordingly.
 *
 * @return Result of valB <OPL> valA
 */
unsigned int performOpl() {
    int a = (int) E.valA;	// XXX: should these be cast?
    int b = (int) E.valB;
    int result = 0;

    if (changeCC) {
        // The CC register is cleared before every OPL
        clearCC();

        // perform addl
        if (E.ifun == ADDL) {
            result = b + a;

            if (result == 0) {
                setCC(ZF, 1);
            }

            if (result < 0) {
                setCC(SF, 1);
            }

            if ((a < 0 == b < 0) && (result < 0 != a < 0)) {
                setCC(OF, 1);
            }
        }

        // perform subl
        if (E.ifun == SUBL) {
            result = b - a;

            if (result == 0) {
                setCC(ZF, 1);
            }

            if (result < 0) {
                setCC(SF, 1);
            }

            if ((a > 0 != b > 0) && (result > 0 == a > 0)) {
                setCC(OF, 1);
            }
        }

        // perform andl
        if (E.ifun == ANDL) {
            result = b & a;

            if (result == 0) {
                setCC(ZF, 1);
            }

            if (result < 0) {
                setCC(SF, 1);
            }
        }

        // perform xorl
        if (E.ifun == XORL) {
            result = b ^ a;

            if (result == 0) {
                setCC(ZF, 1);
            }

            if (result < 0) {
                setCC(SF, 1);
            }
        }
    }

    return result;
}

/**
 * Perform popl instruction
 *
 * @return Modified value for the stack pointer
 */
unsigned int performPopl() {
    return E.valB + 4;
}

/**
 * Perform pushl instruction
 *
 * @return Modified value for the stack pointer
 */
unsigned int performPushl() {
    return E.valB - 4;
}

/**
 * Perform jxx instruction
 *
 * @return 0
 */
unsigned int performJXX() {
    return 0;
}

/**
 * Perform call instruction
 *
 * @return Modified value for the stack pointer
 */
unsigned int performCall() {
    return E.valB - 4;
}

/**
 * Perform ret instruction
 *
 * @return Modified value for the stack pointer
 */
unsigned int performRet() {
    return E.valB + 4;
}

/**
 * Determine if the M register should be stalled. According to HCL,
 * M will never be stalled, therefore it returns false.
 */
bool M_stall() {
    return FALSE;
}

/**
 * Determine if M needs to be bubbled based on status forwarded
 * from memory and writeback stages.
 *
 * @param status Holds status values from later stages
 * @return True if M should be bubbled, false otherwise
 */
bool M_bubble(statusType status) {
    bool bubble = FALSE;

    if ((status.m_stat == SADR || status.m_stat == SINS || status.m_stat == SHLT) ||
        (status.W_stat == SADR || status.W_stat == SINS || status.W_stat == SHLT)) {
        bubble = TRUE;
    }

    return bubble;
}
