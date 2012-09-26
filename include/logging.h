#ifndef LOGGING_H
#define LOGGING_H 1

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define NONE	0x00
#define ERROR	0x01
#define INFO	0x02
#define FINE	0x04
#define FINER	0x08
#define FINEST	0x10
#define DEBUG	0x20
#define TEST_DEBUG	0x40

#define LEVEL TEST_DEBUG /* logging level */

#define LOG_BUFFER_LEN 256

void write_log(int, char*, ...);

#endif /* LOGGING_H */
