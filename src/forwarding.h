/* 
 * forwarding.h
 */

#ifndef FORWARDING_H
#define FORWARDING_H

typedef struct {
    unsigned int e_dstE;
    unsigned int e_valE;
    unsigned int M_dstM;
    unsigned int m_valM;
    unsigned int M_dstE;
    unsigned int M_valE;
    unsigned int M_valA;
    unsigned int M_Cnd;
    unsigned int M_icode;
    unsigned int W_icode;
    unsigned int W_dstM;
    unsigned int W_valM;
    unsigned int W_valE;
    unsigned int W_dstE;
} forwardType;

#endif  /* FORWARDING_H */

