/*
 * Copyright (c) 2020 Scott Bennett <scottb@fastmail.com>
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

#include <limits.h>
#include <stdio.h>
#include <unistd.h>

#include "bool.h"
#include "registers.h"
#include "logger.h"

void
clearRegisters_setsallzeros(int spotcheckreg)
{
	clearRegisters();
	rregister reg = { spotcheckreg };
	printf("reg:\t%d\n", reg.reg);

	unsigned int result;
	result = getRegister(reg);

	printf("output:\t%08x\n", result);
	if (result != 0) {
		printf("===> test failed!\n");
	}
}

void
setRegister_badreg_notstorevalue(int reg, unsigned int val)
{
	printf("reg:\t%d\n", reg);
	printf("val:\t%08x\n", val);

	rregister r = { reg };
	setRegister(r, val);

	for (int i = 0; i < REGSIZE; i++) {
		r.reg = i;
		unsigned int result = getRegister(r);

		if (result != 0) {
			printf("result:\treg %d: %08x\n", i, result);
			printf("===> test failed!\n");
		}
	}
}

void
setRegister_validinput_storesvalue(int reg, unsigned int val)
{
	printf("reg:\t%d\n", reg);
	printf("val:\t%08x\n", val);

	rregister r = { reg };
	setRegister(r, val);

	unsigned int result = getRegister(r);
	printf("result:\treg %d: %08x\n", r.reg, result);

	if (result != val) {
		printf("===> test failed!\n");
	}
}

int
main(int argc, char **argv)
{
	int ch;
	while ((ch = getopt(argc, argv, "d")) != -1) {
		switch (ch) {
		case 'd':
			/* show debug log output */
			log_init(2, 0);
			break;
		default:
			printf("bad option... exiting\n");
			return 1;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc > 0) {
		printf("too many args... exiting\n");
		return 1;
	}

	/* begin tests */
	printf("Test: clearRegisters_setsallzeros\n\n");
	clearRegisters_setsallzeros(EAX);
	clearRegisters_setsallzeros(EDI);

	printf("\n");
	printf("Test: setRegister_badreg_notstorevalue\n\n");
	setRegister_badreg_notstorevalue(9, 5);
	setRegister_badreg_notstorevalue(RNONE, 127);
	setRegister_badreg_notstorevalue(-1, 16);
	setRegister_badreg_notstorevalue(INT_MIN, 200);
	setRegister_badreg_notstorevalue(INT_MAX, 200);

	printf("\n");
	printf("Test: setRegister_validinput_storesvalue\n\n");
	setRegister_validinput_storesvalue(EAX, 5);
	setRegister_validinput_storesvalue(EDX, 127);
	setRegister_validinput_storesvalue(ESI, 16);
	setRegister_validinput_storesvalue(ESP, INT_MAX);

	/* test clearing registers again for good measure */
	printf("\n");
	printf("Test: clearMemory_setsallzeros\n\n");
	clearRegisters_setsallzeros(EAX);
	clearRegisters_setsallzeros(EDI);
}
