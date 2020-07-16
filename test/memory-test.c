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
	printf("addr:\t%08x\n", spotcheckaddr);

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

int
main()
{
	printf("Test: clearMemory_setsallzeros\n\n");
	clearMemory();
	clearMemory_setsallzeros(0);
	clearMemory_setsallzeros(4092);
	
	const int basz = 4;
	int badaddresses[4] = { -1, 4096, INT_MAX, INT_MIN };
	int i;

	printf("\n");
	printf("Test: getByte_badaddress_setsmemerror\n\n");
	for (i = 0; i < basz; i++) {
		getByte_badaddress_setsmemerror(badaddresses[i]);
	}

	printf("\n");
	printf("Test: getByte_badaddress_returnszero\n\n");
	for (i = 0; i < basz; i++) {
		getByte_badaddress_returnszero(badaddresses[i]);
	}

	printf("\n");
	printf("Test: putByte_badaddress_setsmemerror\n\n");
	for (i = 0; i < basz; i++) {
		putByte_badaddress_setsmemerror(badaddresses[i]);
	}

	return 0;
}
