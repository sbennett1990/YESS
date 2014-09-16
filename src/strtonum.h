/* 
 * File:   strtonum.h
 * Author: Scott Bennett
 */

#ifndef STRTONUM_H
#define STRTONUM_H

long
strtonum(const char *numstr, long minval, long maxval, 
		const char **errstr, int base);

#endif /* STRTONUM_H */
