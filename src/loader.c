/**
 * File:   loader.c
 * Author: Alex Savarda & Scott Bennett
 * 
 * Contains functions for loading machine source
 * code into the YESS memory. Also performs 
 * error checking on the source file.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "bool.h"
#include "loader.h"
#include "memory.h"
#include "tools.h"

// Prototypes for "private" functions
static bool validFileName(char * fileName);
static bool checkLine(char * record, int prevAddr);
static bool isAddress(char * record);
static bool isData(char * record);
static bool isSpaces(char * record, int start, int end);
static bool checkAddress(char * record, int prevAddr);
static bool checkData(char * record);
static bool checkHex(char * record, int start, int end);
static void discardRest(FILE * filePtr);
static int grabAddress(char * record);
static unsigned char grabDataByte(char * record, int start);
static int numBytes(char * record);

/**
 * Driver function for the entire YESS program. Takes in
 * a machine code source file and loads the instructions
 * and data into memory.
 * 
 * @param argv    Number of arguments
 * @param *args[] List of command line arguments
 * @return True if load was successful, false if error occured
 */
bool load(int argv, char * args[]) {
    char * fileName = args[1];
    FILE * file;  // pointer to the actual file
    char record[80];  
    bool memError;

    // Check for valid file name
    if(argv != 2 || !validFileName(fileName)) {
        printf("\nFile opening failed");
        printf("\nUsage: yess <filename>.yo\n");
        return FALSE; /*** exit function ***/
    }

    // Open file for reading
    file = fopen(fileName, "r");
    
    // Check if file was not opened
    if (file == NULL) {
        printf("\nFile opening failed");
        printf("\nUsage: yess <filename>.yo\n");
        fclose(file);
        return FALSE; /*** exit function ***/
    }

    int prevAddr = -1;  // initial value since no address has been modified yet
    int lineNo = 1;
    int byteAddress;  // one byte address in memory [0..4095]
    int numberOfBytes;
    unsigned char dataByte; // one byte from record to store in memory
    
	// Attempt to load each line in file into memory
    while (fgets(record, 80, file) != NULL) {
        // Check if line is <= 80 characters
        int len = strlen(record);
        if (record[len-1] != '\n')
            discardRest(file);

        // Error checking...
        if (checkLine(record, prevAddr)) {
        
            if (isAddress(record)) {
                byteAddress = grabAddress(record);
                
                if (isData(record)) {
                    // int byteNumber; // = numberOfBytes;
                    numberOfBytes = numBytes(record);
		    
                    int byteNumber;
                    for (byteNumber = 1; byteNumber <= numberOfBytes; byteNumber++) {
                        dataByte = grabDataByte(record, WHICH_BYTE(byteNumber));
                        
                        putByte(byteAddress, dataByte, &memError);
                        
                        // Check for a memory error
                        if (memError) {
                            fclose(file);
                            return FALSE; /*** exit function ***/
                        }
                        
                        byteAddress++;
                    }
                
                    prevAddr = byteAddress - 1;
                }
            }
            
            lineNo++;
        }
        // there was an error in the record
        else {
            printf("Error on line %d\n", lineNo);
            
            int i;
            for (i = 0; i < strlen(record); i++) {
                printf("%c", record[i]);
            }
            printf("\n");
            
            fclose(file);
            return FALSE; /*** exit function ***/
        }
    }

    // close file
    fclose(file);
    return TRUE;
}

/**
 * Checks if the file name ends in ".yo".
 *
 * @param *fileName Pointer to a string to be checked
 * @return Returns true if file ends in ".yo"
 */
bool validFileName(char * fileName) {
    int len = strlen(fileName);

    if(fileName[len-1] == 'o' &&
       fileName[len-2] == 'y' &&
       fileName[len-3] == '.') {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * Checks if the record is in the correct format.
 *
 * @param *record  One record to be checked
 * @param prevAddr The previously written-to memory address
 * @return True if the line is correct
 */
bool checkLine(char * record, int prevAddr) {
    bool b = FALSE;

    // Check for the pipe character, since it's in every line
    if (record[22] != '|')
        return b;  // No pipe character present

    // Data record
    if (isAddress(record)) {
        // Check all space character placements
        if (isSpaces(record, 0, 1) && isSpaces(record, 8, 8) &&
              isSpaces(record, 21, 21)) {
            // Check for address correctness
            if (checkAddress(record, prevAddr)) {
                b = TRUE;

                if (isData(record))
                    b = checkData(record);
            }
        }
    }
    // Comment record
    else {
        if (isSpaces(record, 0, 21))
            b = TRUE;
    }

    return b;
}

/**
 * Checks to see if the record contains an address. No
 * error checking is done on the address, but the syntax
 * of the address is checked.
 *
 * @param record A record
 * @return True if record has an address
 */
bool isAddress(char * record) {
    if (record[2] == '0' &&
        record[3] == 'x' &&
        record[7] == ':') {
        return TRUE;
    } else
        return FALSE;
}

/**
 * Checks to see if the record contains data. No error
 * checking is performed. Data should be stored in
 * columns 9 through 20 as hex, with 0 to 6 bytes.
 *
 * @param *record Record to be checked
 * @return True if record contains data
 */
bool isData(char * record) {
    // since no error checking is performed, only examine
    // the first index where data should be
    if (isxdigit(record[9]))
        return TRUE;
    else
        return FALSE;
}

/**
 * Checks to see if there are spaces at start index
 * through end index.
 *
 * @param *record A record to be checked
 * @param start   Start index
 * @param end     End index
 * @return True if record contains spaces at indices
 */
bool isSpaces(char * record, int start, int end) {
    int i;
    for (i = start; i <= end; i++) {
        if (record[i] != ' ')
            return FALSE;
    }
	
    return TRUE;
}

/**
 * Checks if the address in the data record is formatted
 * correctly. Also checks if the address in the data
 * record is greater than the previous address of memory
 * where data was stored. If prevAddr is -1, then there
 * was no previous address. This function assumes that
 * the record being checked does contain an address.
 *
 * @param *record  A record with an address
 * @param prevAddr The previously written-to memory address
 * @return True if the address is correct
 */
bool checkAddress(char * record, int prevAddr) {
    if (checkHex(record, 4, 6)) {

        if (prevAddr == -1)
            return TRUE;
        else {
            // current must be > prevAddr
            int currentAddr = grabAddress(record);

            if (currentAddr > prevAddr)
                return TRUE;
            else
                return FALSE;
        }
    } else // address not formatted correctly
        return FALSE;
}

/**
 * Checks if the data in the record is in the correct
 * format. Does not check for a hex digit in column
 * 21 since there is supposed to be a space character.
 * If there is a hex digit in column 21 the error
 * will be caught elsewhere. This function assumes
 * the record is supposed to contain data.
 *
 * @param *record A record to be checked
 * @return True if the data is correctly formatted
 */
bool checkData(char * record) {
    // data must be in at least columns 9 & 10
    if (checkHex(record, 9, 10)) {
	int i;
        for (i = 11; i <= 20; i += 2) {
            if (isxdigit(record[i])) {
                if (!isxdigit(record[i+1])) { // next digit should be hex
                    return FALSE;
                }
            } else // no further data
                return TRUE;
        }
    } else // no data present, or incorrect placement
        return FALSE;
}

/**
 * Checks to see if record contains hex from start index
 * through end index.
 *
 * @param *record One record to be checked
 * @param start   Start index
 * @param end     End index
 * @return True if every digit is a hex digit
 */
bool checkHex(char * record, int start, int end) {
    int i;
    for (i = start; i <= end; i++) { // traverse record
        if (!(isxdigit(record[i])))
            return FALSE;  // Break out of function if hex digit not found
    }

    return TRUE; // every digit is a hex digit
}

/**
 * If fgets didn't read a newline, then the whole line wasn't read.
 * Read and discard characters until newline is read.
 *
 * @param *filePtr Pointer to the file
 */
void discardRest(FILE * filePtr) {
    // remove chars in file
    while (fgetc(filePtr) != '\n' && !feof(filePtr)) {
        // rest is discarded with no additional statements
    }
}


/**
 * Get the address out of the data record. This
 * function performs no error checking and assumes
 * the record is in fact a data record.
 *
 * @param *record A record with an address
 * @return The address in base 10
 */
int grabAddress(char * record) {
    return (int)strtol(record, NULL, HEX);
}

/**
 * Returns one byte of data at the index in record.
 *
 * @param *record Data to be searched
 * @param start   Start index
 * @return One byte of data from record
 */
unsigned char grabDataByte(char * record, int start) {
    char byte[3];
    
    byte[0] = record[start];
    byte[1] = record[start + 1];
    byte[2] = '\0';
    
    return (unsigned char) strtol(byte, NULL, HEX);
}

/**
 * Calculate the number of bytes of data are in
 * the record [0..6].
 * 
 * @param *record A record with data
 * @return The number of bytes of data
 */
int numBytes(char * record) {
    int start = 9;
    int end = 10;
    int numBytes = 0;

    while (checkHex(record, start, end)) {
        start += 2;
        end += 2;
        numBytes++;
    }

    return numBytes;
}
 
