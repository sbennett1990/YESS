/* 
 * File:   fetchStage.h
 * Author: Cindy Norris
 */

#ifndef FETCHSTAGE_H
#define	FETCHSTAGE_H

typedef struct {
    unsigned int predPC;
} fregister;

struct F {
    unsigned int predPC;
};


// prototypes for functions called from files other than fetchStage
fregister getFregister(void);
void clearFregister(void);
void fetchStage();
#endif	/* FETCHSTAGE_H */

