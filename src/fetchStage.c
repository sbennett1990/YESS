/*
 * File:   fetchStage.c
 * Author: Scott Bennett
 */

#include "bool.h"
#include "tools.h"
#include "memory.h"
#include "registers.h"
#include "instructions.h"
#include "fetchStage.h"
#include "decodeStage.h"

/*
 * F register holds the input for the fetch stage.
 */
static struct fregister F;

// Prototypes for "private" functions
static unsigned int selectPC(forwardType forward, unsigned int);
static unsigned int predictPC(unsigned int icode, unsigned int valC,
    unsigned int valP);
static unsigned int pcIncrement(unsigned int f_pc, unsigned int icode);
static bool instructionValid(unsigned int icode);
static unsigned int getIcode(unsigned int memByte, bool memError);
static unsigned int getIfun(unsigned int memByte, bool memError);
static bool needRegids(unsigned int icode);
static bool needValC(unsigned int icode);
static unsigned int getValC(unsigned int f_pc, bool * memError);
static bool bubbleF(void);
static bool stallF(controlType control);
static bool bubbleD(controlType control);
static bool stallD(controlType control);

/*
 * Return a copy of the F register.
 *
 * Return an fregister
 */
struct fregister
getFregister()
{
    return F;
}

/*
 * Clear the F register
 */
void
clearFregister()
{
    clearBuffer((char *) &F, sizeof(F));
}

/*
 * Fetch an instruction from memory and update the D register
 * accordingly.
 *
 * Parameters:
 *  forward     holds values forwarded from previous stages
 *  control     holds values forwarded from later stages
 */
void
fetchStage(forwardType forward, controlType control)
{
    bool memError = FALSE;
    unsigned int f_pc = selectPC(forward, F.predPC);
    unsigned int stat = SAOK;
    unsigned int icode;
    unsigned int ifun;
    unsigned int rA = RNONE;
    unsigned int rB = RNONE;
    unsigned int valC = 0;
    unsigned int valP = 0;
    unsigned int memByte;
    unsigned int tempPC;

    memByte = getByte(f_pc, &memError);

    if (memError) {
        stat = SADR;
    }

    icode = getIcode(memByte, memError);
    ifun = getIfun(memByte, memError);

    if (instructionValid(icode)) {
        if (icode == HALT) {
            stat = SHLT;
        }

        // Get register ids if necessary
        if (needRegids(icode)) {
            tempPC = f_pc + 1;

            // get rA and rB from the next byte of memory
            memByte = getByte(tempPC, &memError);

            if (memError) {
                stat = SADR;
            }

            rA = getBits(4, 7, memByte);
            rB = getBits(0, 3, memByte);
        }

        // Get valC if necessary
        if (needValC(icode)) {
            tempPC = f_pc;

            if (needRegids(icode)) {
                tempPC += 1;
            }

            valC = getValC(tempPC, &memError);

            if (memError) {
                stat = SADR;
            }
        }
    }
    else {
        stat = SINS;
        F.predPC = F.predPC + 1;
    }

    // Calculate valP
    valP = pcIncrement(f_pc, icode);

    // Stall F? keep previous value
    if (stallF(control)) {
        F.predPC = f_pc;
    }
    else {
        F.predPC = predictPC(icode, valC, valP);
    }

    // Stall or bubble D?
    if (bubbleD(control)) {
        // Insert a NOP
        updateDregister(SAOK, NOP, 0, RNONE, RNONE, 0, valP);
    }
    else if (!stallD(control)) {
        // Update D as normal (do not stall)
        updateDregister(stat, icode, ifun, rA, rB, valC, valP);
    }
	// else do nothing because D should be stalled
}

/*
 * Select the source of the PC.
 *
 * Parameters:
 *  forward     holds values forwarded from previous stages
 *  predPC	current value of F.predPC
 *
 * Return source value for the PC
 */
unsigned int
selectPC(forwardType forward, unsigned int predPC)
{
    // Uses forwarded M_valA, W_valM
    // Mispredicted branch. Fetch at incremented PC
    if (forward.M_icode == JXX && !(forward.M_Cnd)) {
        return forward.M_valA;
    }

    // Completion of RET instruction
    if (forward.W_icode == RET) {
        return forward.W_valM;
    }

    // Default
    return predPC;
}

/*
 * Predict the value of the next instruction to be
 * executed.
 *
 * Parameters:
 *  icode   the instruction code
 *  valC    a constant word, part of the instruction
 *  valP    the address of next sequential instruction
 *          in memory
 *
 * Return the predicted PC
 */
unsigned int
predictPC(unsigned int icode, unsigned int valC, unsigned int valP)
{
	if (icode == JXX || icode == CALL) {
		return valC;
	}
	else {
		return valP;
	}
}

/*
 * Determine the address of the next sequential instruction in
 * memory.
 *
 * Parameters:
 *  f_pc    current value of the PC
 *  icode   the instruction code
 *
 * Return the address of the next sequential instruction
 */
unsigned int
pcIncrement(unsigned int f_pc, unsigned int icode)
{
    unsigned int valP;

    switch (icode) {
        case HALT:
        case NOP:
        case RET:
            valP = f_pc + 1;
            break;

        case OPL:
        case RRMOVL:
        case POPL:
        case PUSHL:
            valP = f_pc + 2;
            break;

        case DUMP:
        case JXX:
        case CALL:
            valP = f_pc + 5;
            break;

        case IRMOVL:
        case RMMOVL:
        case MRMOVL:
            valP = f_pc + 6;
            break;

        default:
            valP = f_pc + 1;
    }

    return valP;
}

/*
 * Determine the icode of the fetched instruction.
 *
 * Parameters:
 *  memByte     a byte that should contain the instruction code
 *  memError    specifies whether a memory error occurred
 *
 * Return the instruction code, or a NOP for a memory error
 */
unsigned int
getIcode(unsigned int memByte, bool memError)
{
    if (memError) {
        return NOP;
    } else {
        return getBits(4, 7, memByte);
    }
}

/*
 * Determine the ifun of the fetched instruction.
 *
 * Parameters:
 *  memByte     a byte that should contain the instruction function
 *  memError    specifies whether a memory error occurred
 *
 * Return the instruction function, or FNONE for a memory error
 */
unsigned int
getIfun(unsigned int memByte, bool memError)
{
    if (memError) {
        return 0;
    } else {
        return getBits(0, 3, memByte);
    }
}

/*
 * Test whether the fetched instruction is valid.
 *
 * Parameters:
 *  icode   the instruction code
 *
 * Return true if icode is valid; false otherwise
 */
bool
instructionValid(unsigned int icode)
{
    bool valid = FALSE;

    switch (icode) {
        case HALT:
        case NOP:
        case DUMP:
        case IRMOVL:
        case OPL:
        case RRMOVL:
        case RMMOVL:
        case MRMOVL:
        case POPL:
        case PUSHL:
        case RET:
        case JXX:
        case CALL:
            valid = TRUE;
            break;

        default:
            valid = FALSE;
    }

    return valid;
}

/*
 * Does fetched instruction require a regid byte?
 *
 * Parameters:
 *  icode   the instruction code
 *
 * Return true if icode requires a regid; false otherwise
 */
bool
needRegids(unsigned int icode)
{
    bool need = FALSE;

    switch (icode) {
        case RRMOVL:
        case OPL:
        case PUSHL:
        case POPL:
        case IRMOVL:
        case RMMOVL:
        case MRMOVL:
            need = TRUE;
            break;

        default:
            need = FALSE;
    }

    return need;
}

/*
 * Does fetched instruction require a constant word (valC)?
 *
 * Parameters:
 *  icode   the instruction code
 *
 * Return true if instruction needs a valC; false otherwise
 */
bool
needValC(unsigned int icode)
{
    bool need = FALSE;

    switch (icode) {
        case DUMP:
        case IRMOVL:
        case RMMOVL:
        case MRMOVL:
        case JXX:
        case CALL:
            need = TRUE;
            break;

        default:
            need = FALSE;
    }

    return need;
}

/*
 * Get and build the constant word, valC, from memory.
 *
 * Parameters:
 *  f_pc        the current value of the PC
 *  *memError   pointer to memory error status
 *
 * Return the constant word, valC
 */
unsigned int
getValC(unsigned int f_pc, bool * memError)
{
    unsigned char byte0, byte1, byte2, byte3;

    byte0 = getByte(f_pc + 1, memError);
    byte1 = getByte(f_pc + 2, memError);
    byte2 = getByte(f_pc + 3, memError);
    byte3 = getByte(f_pc + 4, memError);

    return buildWord(byte0, byte1, byte2, byte3);
}

/*
 * Determine whether F should be bubbled.
 * According to HCL, F will never be bubbled,
 * therefore it returns false.
 */
bool
bubbleF()
{
    return FALSE;
}

/*
 * Determine if F should be stalled based on input forwarded
 * by later stages.
 *
 * Parameters:
 *  control     holds values from later stages
 *
 * Return true if F need to be stalled; false otherwise
 */
bool
stallF(controlType control)
{
    bool stall = FALSE;

    if (((control.E_icode == MRMOVL || control.E_icode == POPL)
         && (control.E_dstM == control.d_srcA || control.E_dstM == control.d_srcB))
        || (control.D_icode == RET || control.E_icode == RET
            || control.M_icode == RET)) {
        stall = TRUE;
    }

    return stall;
}

/*
 * Determine if D needs to be bubbled based on input forwarded
 * by later stages.
 *
 * Parameters:
 *  control     holds values from later stages
 *
 * Return true if D should be bubbled; false otherwise
 */
bool
bubbleD(controlType control)
{
    bool bubble = FALSE;

    // conditions for mispredicted branch
    if ((control.E_icode == JXX && !control.e_Cnd)
        ||
        (!(stallD(control))
         && (RET == control.D_icode || RET == control.E_icode
             || RET == control.M_icode))) {
        bubble = TRUE;
    }

    return bubble;
}

/*
 * Determine if D needs to be stalled based on input forwarded
 * by later stages.
 *
 * Parameters:
 *  control     holds values from later stages
 *
 * Return true if D should be stalled; false otherwise
 */
bool
stallD(controlType control)
{
    bool stall = FALSE;

    // conditions for load/use hazard
    if ((control.E_icode == MRMOVL || control.E_icode == POPL)
        &&
        (control.E_dstM == control.d_srcA || control.E_dstM == control.d_srcB)) {
        stall = TRUE;
    }

    return stall;
}
