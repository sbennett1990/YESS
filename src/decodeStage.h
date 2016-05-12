/*
 * File:   decodeStage.h
 * Author: Scott Bennett
 */

#ifndef DECODESTAGE_H
#define	DECODESTAGE_H

typedef struct {
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int rA;
    unsigned int rB;
    unsigned int valC;
    unsigned int valP;
} dregister;

struct D {
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int rA;
    unsigned int rB;
    unsigned int valC;
    unsigned int valP;
};


dregister getDregister(void);
void clearDregister(void);
void decodeStage();
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun,
                     unsigned int rA, unsigned int rB, unsigned int valC,
                     unsigned int valP);

#endif	/* DECODESTAGE_H */

