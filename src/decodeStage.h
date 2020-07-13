/*
 * Copyright (c) 2014, 2016, 2020 Scott Bennett <scottb@fastmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef	DECODESTAGE_H
#define	DECODESTAGE_H

#include "forwarding.h"
#include "control.h"
#include "registers.h"

/*
 * +----+---------------------------------------------------------------------+
 * | D  | icode | ifun  |      | rA | rB |   valC   |   valP   |              |
 * +----+---------------------------------------------------------------------+
 */
struct dregister {
	unsigned int stat;
	unsigned int icode;
	unsigned int ifun;
	rregister rA;		// Program Register
	rregister rB;		// Program Register
	unsigned int valC;
	unsigned int valP;
};

struct dregister getDregister(void);
void clearDregister(void);
void decodeStage(forwardType, controlType *);
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun,
    rregister rA, rregister rB, unsigned int valC, unsigned int valP);

#endif	/* DECODESTAGE_H */
