#ifndef LOGGING_H
#define LOGGING_H 1

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <arpa/inet.h>

#define NONE		0x00
#define ERROR		0x01
#define INFO		0x02
#define FINE		0x04
#define FINER		0x08
#define FINEST		0x10
#define DEBUG		0x20
#define MEM_DEBUG	0x40

#define LEVEL DEBUG /* logging level */

#define LOG_BUFFER_LEN 255

void log_ln(int, char*, ...);
void log_conn(int, struct sockaddr_in*, char*, ...);

#endif /* LOGGING_H */
