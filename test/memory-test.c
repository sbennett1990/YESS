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

#include "bool.h"
#include "memory.h"


void
clearMemory_setsallzeros(int spotcheckaddr)
{
	printf("addr:\t%03x\n", spotcheckaddr);

	unsigned int result;
	bool memError;
	result = getWord(spotcheckaddr, &memError);

	printf("output:\t%d\n", result);
	if (result != 0) {
		printf("===> test failed!\n");
	}
	if (memError) {
		printf("===> memError! that's a problem...\n");
	}
}

void
getByte_badaddress_setsmemerror(int byteAddr)
{
	printf("input:\t%08x\n", byteAddr);

	unsigned char result;
	bool memError;
	result = getByte(byteAddr, &memError);

	printf("output:\tmemError = %d\n", memError);
	if (!memError) {
		printf("===> test failed!\n");
	}
}

void
getByte_badaddress_returnszero(int byteAddr)
{
	printf("input:\t%08x\n", byteAddr);

	unsigned char result;
	bool memError;
	result = getByte(byteAddr, &memError);

	printf("output:\t%d\n", result);
	if (result != 0) {
		printf("===> test failed!\n");
	}
}

void
putByte_badaddress_setsmemerror(int byteAddr)
{
	printf("input:\t%08x\n", byteAddr);

	bool memError;
	putByte(byteAddr, 0, &memError);

	printf("output:\tmemError = %d\n", memError);
	if (!memError) {
		printf("===> test failed!\n");
	}
}

void
putByte_validinput_storesvalue(int byteAddr, unsigned char val)
{
	printf("addr:\t%03x\n", byteAddr);
	printf("val:\t %02x\n", val);

	bool memError;
	putByte(byteAddr, val, &memError);
	if (memError) {
		printf("===> memError on putByte()! that's a problem...\n");
	}

	unsigned char result = getByte(byteAddr, &memError);
	printf("result:\tAddr 0x%03x: %02x\n", byteAddr, result);
	if (result != val) {
		printf("===> test failed!\n");
	}
	if (memError) {
		printf("===> memError! that's a problem...\n");
	}
}

int
main()
{
	printf("Test: clearMemory_setsallzeros\n\n");
	clearMemory();
	clearMemory_setsallzeros(0);
	clearMemory_setsallzeros(4092);

#define BASZ	4
	int badaddresses[BASZ] = { -1, 4096, INT_MAX, INT_MIN };
	int i;

	printf("\n");
	printf("Test: putByte_badaddress_setsmemerror\n\n");
	for (i = 0; i < BASZ; i++) {
		putByte_badaddress_setsmemerror(badaddresses[i]);
	}

	printf("\n");
	printf("Test: putByte_validinput_storesvalue\n\n");
	putByte_validinput_storesvalue(0, 5);
	putByte_validinput_storesvalue(100, 127);
	putByte_validinput_storesvalue(4095, 16);
	putByte_validinput_storesvalue(1001, 200);
	putByte_validinput_storesvalue(2048, 255);

	printf("\n");
	printf("Test: getByte_badaddress_setsmemerror\n\n");
	for (i = 0; i < BASZ; i++) {
		getByte_badaddress_setsmemerror(badaddresses[i]);
	}

	printf("\n");
	printf("Test: getByte_badaddress_returnszero\n\n");
	for (i = 0; i < BASZ; i++) {
		getByte_badaddress_returnszero(badaddresses[i]);
	}

	return 0;
}
