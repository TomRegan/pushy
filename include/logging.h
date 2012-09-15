#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define LEVEL 0x20 /* logging level */

#define ERROR	0x01
#define INFO	0x02
#define FINE	0x04
#define FINER	0x08
#define FINEST	0x10
#define DEBUG	0x20

#define LOG_BUFFER_LEN 256

void write_log(int, char*, ...);
