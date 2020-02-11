/*
 * File: fetchStage.h
 */

#ifndef	FETCHSTAGE_H
#define	FETCHSTAGE_H

#include "forwarding.h"
#include "control.h"

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
void fetchStage(forwardType, controlType);

#endif	/* FETCHSTAGE_H */
