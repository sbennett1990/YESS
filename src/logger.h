/*
 * logger.h
 */

void log_init(int, int);
void log_procinit(const char *);
void log_verbose(int);
void log_warn(const char *, ...);
void log_warnx(const char *, ...);
void log_info(const char *, ...);
void log_debug(const char *, ...);
void logit(int, const char *, ...);
void vlog(int, const char *, va_list);
void fatal(const char *, ...);
void fatalx(const char *, ...);
