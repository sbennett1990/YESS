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
#include <stdlib.h>
#include <unistd.h>

#include "bool.h"
#include "memory.h"
#include "logger.h"

int num_failed = 0;

void
clearMemory_setsallzeros(int spotcheckaddr)
{
	clearMemory();
	printf("addr:\t%03x\n", spotcheckaddr);

	unsigned int result;
	bool memError;
	result = getWord(spotcheckaddr, &memError);

	printf("output:\t%08x\n", result);
	if (result != 0) {
		num_failed++;
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

	uint8_t result;
	bool memError;
	result = getByte(byteAddr, &memError);

	printf("output:\tmemError = %d\n", memError);
	if (!memError) {
		num_failed++;
		printf("===> test failed!\n");
	}
}

void
getByte_badaddress_returnszero(int byteAddr)
{
	printf("input:\t%08x\n", byteAddr);

	uint8_t result;
	bool memError;
	result = getByte(byteAddr, &memError);

	printf("output:\t%d\n", result);
	if (result != 0) {
		num_failed++;
		printf("===> test failed!\n");
	}
}

void
getByte_validinput_returnsval(int byteAddr, uint8_t val)
{
	printf("addr:\t%03x\n", byteAddr);

	uint8_t result;
	bool memError;

	putByte(byteAddr, val, &memError);
	if (memError) {
		printf("===> memError! that's a problem...\n");
	}
	result = getByte(byteAddr, &memError);

	printf("result:\t %02x\n", result);
	if (result != val) {
		num_failed++;
		printf("===> test failed!\n");
	}
	if (memError) {
		printf("===> memError! that's a problem...\n");
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
		num_failed++;
		printf("===> test failed!\n");
	}
}

void
putByte_validinput_storesvalue(int byteAddr, uint8_t val)
{
	printf("addr:\t%03x\n", byteAddr);
	printf("val:\t %02x\n", val);

	bool memError;
	putByte(byteAddr, val, &memError);
	if (memError) {
		printf("===> memError on putByte()! that's a problem...\n");
	}

	uint8_t result = getByte(byteAddr, &memError);
	printf("result:\tAddr 0x%03x: %02x\n", byteAddr, result);
	if (result != val) {
		num_failed++;
		printf("===> test failed!\n");
	}
	if (memError) {
		printf("===> memError! that's a problem...\n");
	}
}

void
getWord_badaddress_setsmemerror(int byteAddr)
{
	printf("input:\t%08x\n", byteAddr);

	unsigned int result;
	bool memError;
	result = getWord(byteAddr, &memError);

	printf("output:\tmemError = %d\n", memError);
	if (!memError) {
		num_failed++;
		printf("===> test failed!\n");
	}
}

void
getWord_badaddress_returnszero(int byteAddr)
{
	printf("input:\t%08x\n", byteAddr);

	unsigned int result;
	bool memError;
	result = getWord(byteAddr, &memError);

	printf("output:\t%d\n", result);
	if (result != 0) {
		num_failed++;
		printf("===> test failed!\n");
	}
}

void
getWord_validinput_returnsword(int byteAddr)
{
	/* TODO */
}

void
integration(int byteAddr, short offset)
{
	printf("addr:\t%03x\n", byteAddr);

	bool memError;
	unsigned int v = 0xddccbbaa;
	putWord(byteAddr, v, &memError);
	if (memError) {
		printf("===> memError! that's a problem...\n");
	}

	printf("input:\t%08x\n", v);

	putByte(byteAddr + offset, 7, &memError);
	if (memError) {
		printf("===> memError! that's a problem...\n");
	}

	unsigned int result = getWord(byteAddr, &memError);
	if (memError) {
		printf("===> memError! that's a problem...\n");
	}

	printf("output:\t%08x\n", result);
}

int
main(int argc, char **argv)
{
	extern char *malloc_options;
	malloc_options = "CFGJ";

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

	initMemory();

	/* begin tests */
	printf("Test: clearMemory_setsallzeros\n\n");
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

	printf("\n");
	printf("Test: getByte_validinput_returnsval\n\n");
	getByte_validinput_returnsval(0, 10);
	getByte_validinput_returnsval(1024, 16);
	getByte_validinput_returnsval(4091, 9);
	getByte_validinput_returnsval(4095, 255);

	printf("\n");
	printf("Test: getWord_badaddress_setsmemerror\n\n");
	for (i = 0; i < BASZ; i++) {
		getWord_badaddress_setsmemerror(badaddresses[i]);
	}
	/* addresses that are not multiples of 4 */
	getWord_badaddress_setsmemerror(1);
	getWord_badaddress_setsmemerror(5);
	getWord_badaddress_setsmemerror(4095);

	printf("\n");
	printf("Test: getWord_badaddress_returnszero\n\n");
	for (i = 0; i < BASZ; i++) {
		getWord_badaddress_returnszero(badaddresses[i]);
	}
	/* addresses that are not multiples of 4 */
	getWord_badaddress_returnszero(1);
	getWord_badaddress_returnszero(5);
	getWord_badaddress_returnszero(4095);

	printf("\n");
	printf("Test: integration\n\n");
	integration(4, 0);
	integration(4, 3);

	/* test clearing memory again for good measure */
	printf("\n");
	printf("Test: clearMemory_setsallzeros\n\n");
	clearMemory_setsallzeros(0);
	clearMemory_setsallzeros(4092);

	printf("\nfailed tests: %d\n", num_failed);
	return 0;
}
