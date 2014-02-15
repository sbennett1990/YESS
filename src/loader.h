/* 
 * File:   loader.h
 * Author: Scott Bennett
 */

// define's
#define WHICH_BYTE(n)   ((n * 2) + 7)   // Calculate the column of the data byte
#define HEX   16   // Hexidecimal base number

#ifndef LOADER_H
#define	LOADER_H

// function prototypes
bool load(int argv, char * args[]);

#endif	/* LOADER_H */

