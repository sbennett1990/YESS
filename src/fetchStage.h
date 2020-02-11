/*
 * File: fetchStage.h
 */

#ifndef	FETCHSTAGE_H
#define	FETCHSTAGE_H

/*
 * +----+---------------------------------------------------------------------+
 * | F  |          | predPC |                                                 |
 * +----+---------------------------------------------------------------------+
 */
struct fregister {
	unsigned int predPC;
};

struct fregister getFregister(void);
void clearFregister(void);
void fetchStage();

#endif	/* FETCHSTAGE_H */
