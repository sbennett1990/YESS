/* 
 * File:   fetchStage.h
 * Author: Cindy Norris
 */

#ifndef FETCHSTAGE_H
#define	FETCHSTAGE_H

struct fregister {
    unsigned int predPC;
};


struct fregister getFregister(void);
void clearFregister(void);
void fetchStage();

#endif	/* FETCHSTAGE_H */
