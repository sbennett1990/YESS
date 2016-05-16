/**
 * File:   memoryStage.c
 * Author: Alex Savarda
 */

#include <stddef.h>

#include "bool.h"
#include "tools.h"
#include "memory.h"
#include "instructions.h"
#include "status.h"
#include "control.h"
#include "forwarding.h"
#include "memoryStage.h"
#include "writebackStage.h"
#include "executeStage.h"

// M register holds the input for the execute stage
// It is only accessible from within this file (static)
static mregister M;

// Prototypes for "private" functions
static unsigned int mem_addr(void);
static bool mem_write(void);
static bool mem_read(void);
static bool W_stall(statusType status);
static bool W_bubble(void);

/**
 * Return a copy of the M register
 */
mregister getMregister() {
    return M;
}

/**
 * Clear M register then initialize its icode to NOP and
 * its stat to SAOK.
 */
void clearMregister() {
    clearBuffer((char *) &M, sizeof(M));
    M.icode = NOP;
    M.stat = SAOK;
}

/**
 * May write data to memory, or read data from memory
 * updates W register
 *
 * @param *forward Holds values forwarded to previous stages
 * @param *status  Holds values forwarded to previous stages
 * @param *control Holds values forwarded to previous stages
 */
void memoryStage(forwardType * forward, statusType * status,
                 controlType * control) {
    unsigned int address = mem_addr();
    unsigned int stat = M.stat;
    unsigned int valM = NOADDRESS;
    bool memError = FALSE;

    // read data from memory?
    if (mem_read()) {
        valM = getWord(address, &memError);
    }

    // write data to memory?
    if (mem_write()) {
        putWord(address, M.valA, &memError);
    }

    if (memError) {
        stat = SADR;
    }

    //set values of forwarding, status, and control structs
    forward->M_dstM = M.dstM;
    forward->M_dstE = M.dstE;
    forward->m_valM = valM;
    forward->M_valE = M.valE;
    forward->M_Cnd = M.Cnd;
    forward->M_valA = M.valA;
    forward->M_icode = M.icode;
    status->m_stat = stat;
    control->M_icode = M.icode;

    // Stall W?
    if (!W_stall(*status)) {
        // If stall is true, do nothing to keep current
        // values in writeback Stage
        updateWRegister(stat, M.icode, M.valE, valM, M.dstE, M.dstM);
    }
}

/**
 * Update values in the M register
 */
void updateMRegister(unsigned int stat, unsigned int icode, unsigned int Cnd,
                     unsigned int valE, unsigned int valA, unsigned int dstE,
                     unsigned int dstM) {
    M.stat = stat;
    M.icode = icode;
    M.Cnd = Cnd;
    M.valE = valE;
    M.valA = valA;
    M.dstE = dstE;
    M.dstM = dstM;
}

/**
 * Select memory address
 *
 * @return The memory address. Default is NOADDRESS.
 */
unsigned int mem_addr() {
    unsigned int address = NOADDRESS;

    //set address to valE for these opcodes
    switch (M.icode) {
        case RMMOVL:
        case PUSHL:
        case CALL:
        case MRMOVL:
            address = M.valE;
            break;

        //set address to valA for these opcodes
        case POPL:
        case RET:
            address = M.valA;
            break;

        default:
            address = NOADDRESS;
    }

    return address;
}

/**
 * Set write control signal
 */
bool mem_write() {
    bool write = FALSE;

    //if icode equals MRMOVL, PUSHL, CALL
    //set write to TRUE
    switch (M.icode) {
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

/**
 * Set read control signal
 */
bool mem_read() {
    bool read = FALSE;

    //if icode equals MRMOVL, POPL, RET
    //set read to TRUE
    switch (M.icode) {
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

/**
 * Returns true if the W register should be stalled
 *
 * @param status Holds values of statuses forwarded from later stages
 */
bool W_stall(statusType status) {
    bool stall = FALSE;

    if (status.W_stat == SADR
        || status.W_stat == SINS
        || status.W_stat == SHLT) {
        stall = TRUE;
    }

    return stall;
}

/**
 * Determine if the W register should be bubbled. According to HCL,
 * W will never be bubled, therefore it returns false.
 */
bool W_bubble() {
    return FALSE;
}

