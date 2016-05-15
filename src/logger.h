/*
 * logger.h
 */

#ifndef LOGGER_H
#define LOGGER_H

void log_init(int, int);
void log_verbose(int);
void log_warn(const char *, ...);
void log_warnx(const char *, ...);
void log_info(const char *, ...);
void log_debug(const char *, ...);
void logit(int, const char *, ...);
void fatal(const char *, ...);
void fatalx(const char *, ...);

#endif /* LOGGER_H */
