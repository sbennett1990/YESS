
#ifndef STRTONUM_OBSD_H
#define STRTONUM_OBSD_H

long
strtonum_OBSD(const char *numstr, long minval, long maxval,
         const char **errstr, int base);

#endif /* STRTONUM_OBSD_H */

