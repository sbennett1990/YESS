/*
 * loader.c
 *
 * Loads machine source code (in ASCII form) into YESS memory. Also performs
 * error checking on the source file.
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "loader.h"
#include "logger.h"
#include "tools.h"

#ifdef TEST
#include "mocks/memory_mock.h"
#else
#include "memory.h"
#endif

#define MAXLEN	80
#define RECORDLEN	24

/* Calculate the column of the data byte */
#define WHICH_BYTE(n)	(((n) * 2) + 7)

/* Prototype of strnlen(3), to get rid of compiler warning */
size_t strnlen(const char * s, size_t maxlen);

static bool validateaddress(char * record, int prev_addr);
static bool validatedata(char * record);
static bool validline(char * record, int prevAddr);
static bool hashexdigits(char * record, int start, int end);
static bool hasspaces(char * record, int start, int end);
static int grabAddress(char * record);
static unsigned char grabDataByte(char * record, int start);
static bool isaddress(char * record);
static bool isdata(char * record);
static short numbytes(char * record);
static void discardRest(FILE * filePtr);

/*
 * Driver function for the entire YESS program. Takes in a machine code (ASCII)
 * source file and loads the instructions and data into memory.
 *
 * Parameters:
 *     *fileName    name of the file to load
 *
 * Return true if load was successful; false if error occurred
 */
bool
load(const char * fileName)
{
    FILE * fp;
    char record[MAXLEN];
    char buf[RECORDLEN];
    bool memError;

    if (validatefilename(fileName) == -1) {
        return FALSE; /* EXIT */
    }

    // Open file as read-only
    log_debug("opening file \'%s\'", fileName);
    fp = fopen(fileName, "r");

    if (fp == NULL) {
        log_warn("error opening the file");
        return FALSE; /* EXIT */
    }

    /* initial value since no address has been modified yet */
    int prevaddr = -1;
    int lineno = 1;
    int byteAddress;        // memory address of one byte [0..4095]
    short numberOfBytes;
    unsigned char dataByte; // a byte to store in memory

    // Attempt to load each line in the file into memory
    while (fgets(record, MAXLEN, fp) != NULL) {
        // Check if line is <= 80 characters
        int len = strnlen(record, MAXLEN);

        if (record[len - 1] != '\n') {
            discardRest(fp);
        }

        (void)strncpy(buf, record, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        // Error checking...
        if (!validline(buf, prevaddr)) {
            // there was an error in the record
            printf("Error on line %d\n", lineno);

            for (int i = 0; i < len; i++) {
                printf("%c", record[i]);
            }

            printf("\n");

            fclose(fp);
            return FALSE; /* EXIT */
        }

        if (isaddress(buf)) {
            byteAddress = grabAddress(buf);

            if (isdata(buf)) {
                numberOfBytes = numbytes(buf);

                short byteNumber;

                for (byteNumber = 1; byteNumber <= numberOfBytes; byteNumber++) {
                    dataByte = grabDataByte(buf, WHICH_BYTE(byteNumber));

                    putByte(byteAddress, dataByte, &memError);

                    // Check for a memory error
                    if (memError) {
                        fclose(fp);
                        return FALSE; /* EXIT */
                    }

                    byteAddress++;
                }

                prevaddr = byteAddress - 1;
            }
        }

        lineno++;
    }

    fclose(fp);
    return TRUE;
}

/*
 * Determine if the record contains an address. No
 * error checking is done on the address, but the syntax
 * of the address is checked.
 *
 * Parameters:
 *     *record    one record to check
 *
 * Return true if the record has an address; false otherwise
 */
bool
isaddress(char * record)
{
    int len = strnlen(record, RECORDLEN);

    if (len < 8) {
        return FALSE;
    }

    if (record[2] == '0'
        && record[3] == 'x'
        && record[7] == ':') {
        return TRUE;
    }

    return FALSE;
}

/*
 * Check to see if there are spaces from start index
 * through end index.
 *
 * Parameters:
 *     *record     one record to check
 *     start       starting index
 *     end         ending index
 *
 * Return true if record contains spaces at indices;
 * false otherwise
 */
bool
hasspaces(char * record, int start, int end)
{
    if (start > end) {
        return FALSE;
    }

    int len = strnlen(record, MAXLEN);

    if (len <= end) {
        return FALSE;
    }

    int i;

    for (i = start; i <= end; i++) {
        if (record[i] != ' ') {
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * Check to see if the record contains hex from start index
 * through end index.
 *
 * Parameters:
 *     *record    one record to check
 *     start      the starting index
 *     end        the ending index
 *
 * Return true if every digit is a hex digit; false otherwise
 */
bool
hashexdigits(char * record, int start, int end)
{
    if (start > end) {
        return FALSE;
    }

    int len = strnlen(record, RECORDLEN);

    if (len <= end) {
        return FALSE;
    }

    int i;

    for (i = start; i <= end; i++) {
        if (!(isxdigit(record[i]))) {
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * Get the address out of the data record. This
 * function performs no error checking and assumes
 * the record is in fact a data record.
 *
 * Parameters:
 *     *record    a record with an address
 *
 * Return the address in base 10
 */
int
grabAddress(char * record)
{
    char hex_addr[8] = {
        record[0],
        record[1],
        record[2],
        record[3],
        record[4],
        record[5],
        record[6],
        '\0'
    };

    // TODO: check for the error value -1
    int addr = strtoint(hex_addr, HEX);
    return addr;
}

/*
 * Validate that the address in the data record is formatted
 * correctly. Also check if the address in the data
 * record is greater than the previous address of memory
 * where data was stored. If prev_addr is -1, then there
 * was no previous address. This function assumes that
 * the record being checked does contain an address.
 *
 * Parameters:
 *     *record     a record with an address
 *     prev_addr   the previously written-to memory address
 *
 * Return true if the address is correct; false otherwise
 */
bool
validateaddress(char * record, int prev_addr)
{
    if (!hashexdigits(record, 4, 6)) {
        // address not formatted correctly
        return FALSE;
    }

    if (prev_addr == -1) {
        return TRUE;
    }

    // current must be > prev_addr
    int current_addr = grabAddress(record);

    if (current_addr > prev_addr) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
 * Determine if the record contains data. No error
 * checking is performed. Data should be stored in
 * columns 9 through 20 as hex, with 0 to 6 bytes.
 *
 * Parameters:
 *     *record    one record to check
 *
 * Return true if the record contains data; false otherwise
 */
bool
isdata(char * record)
{
    int len = strnlen(record, RECORDLEN);

    if (len < 10) {
        return FALSE;
    }

    /*
     * since little error checking is performed, only examine
     * the first index where data should be
     */
    if (isxdigit(record[9])) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
 * Validate that the data in the record is in the correct
 * format. Do not check for a hex digit in column
 * 21 since there is supposed to be a space character.
 * If there is a hex digit in column 21 the error
 * will be caught elsewhere. This function assumes
 * the record is supposed to contain data.
 *
 * Parameters:
 *     *record    one record to check
 *
 * Return true if the data is correctly formatted;
 * false otherwise
 */
bool
validatedata(char * record)
{
    int len = strnlen(record, RECORDLEN);

    if (len < 21) {
        return FALSE;
    }

    // data must be in at least columns 9 & 10
    if (!hashexdigits(record, 9, 10)) {
        return FALSE;
    }

    int i;

    for (i = 11; i <= 20; i += 2) {
        if (isxdigit(record[i])) {
            // next column should be a hex digit
            if (!isxdigit(record[i + 1])) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/*
 * Determine if the record (one line from the file) is in the correct format.
 *
 * Parameters:
 *     *record     one record to check
 *     prev_addr   the previously written-to memory address
 *
 * Return true if the line is correctly formatted; false otherwise
 */
bool
validline(char * record, int prev_addr)
{
    int len = strnlen(record, RECORDLEN);

    if (len < 23) {
        return FALSE; /* EXIT */
    }

    // the pipe character is supposed to be on every line
    if (record[22] != '|') {
        return FALSE; /* EXIT */
    }

    if (!isaddress(record)) {
        // this should be a comment record
        if (hasspaces(record, 0, 21)) {
            return TRUE; /* EXIT */
        }
    }

    bool b = FALSE;

    /*
     * this should be a data record - check all space character placements
     */
    if (hasspaces(record, 0, 1)
        && hasspaces(record, 8, 8)
        && hasspaces(record, 21, 21)) {
        // check for address correctness
        if (validateaddress(record, prev_addr)) {
            b = TRUE;

            if (isdata(record)) {
                b = validatedata(record);
            }
        }
    }

    return b;
}

/*
 * Returns one byte of data at the index in the record.
 *
 * Parameters:
 *     *record    the data record to search
 *     start      the starting index
 *
 * Return one byte of data from the record
 */
unsigned char
grabDataByte(char * record, int start)
{
    char byte[3];

    byte[0] = record[start];
    byte[1] = record[start + 1];
    byte[2] = '\0';

    // TODO: check for the error value -1
    return (unsigned char) strtoint(byte, HEX);
}

/*
 * Calculate the number of bytes of data that are in
 * the record (anywhere from 0 to 6 bytes).
 *
 * Parameters:
 *     *record    a record with data
 *
 * Return the number of bytes of data in the record
 */
short
numbytes(char * record)
{
    int len = strnlen(record, RECORDLEN);

    // each line should only have 23 columns of valid information
    if (len < 23) {
        return 0; /* EXIT */
    }

    short start = 9;
    short end = 10;
    short num = 0;

    while (hashexdigits(record, start, end)) {
        start += 2;
        end += 2;
        num += 1;
    }

    return num;
}

/*
 * If fgets didn't read a newline, then the whole line wasn't read.
 * Read and discard characters until a newline is read.
 *
 * Parameters:
 *  *filePtr    pointer to the file
 */
void
discardRest(FILE * filePtr)
{
    // remove chars in file
    while (fgetc(filePtr) != '\n' && !feof(filePtr)) {
        // rest is discarded with no additional statements
    }
}
