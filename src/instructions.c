/*
 * Copyright (c) 2021 Scott Bennett <scottb@fastmail.com>
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

#include <stdint.h>

#include "instructions.h"

/* Instruction Codes */
const icode_t i_nop = { NOP };

/* Status Codes */
const stat_t s_okay = { SAOK };
const stat_t s_halt = { SHLT };
const stat_t s_addr = { SADR };
const stat_t s_inst = { SINS };


/*
 * Determine if the given instruction code type is a specific icode.
 *
 * Parameters:
 *      it     Instruction code type
 *      ic     icode
 */
int
icode_is(icode_t it, uint8_t ic)
{
	return (it.ic == ic);
}
