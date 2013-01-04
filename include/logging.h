/**
 * Copyright 2012 Tom Regan <code.tom.regan@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

/**
 * @file  logging.h
 * @brief  functions for logging events
 * @author  Tom Regan <code.tom.regan@gmail.com>
 */

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

#define LEVEL MEM_DEBUG /* logging level */

#define LOG_BUFFER_LEN 255

void log_ln(int, char*, ...);
void log_conn(int, struct sockaddr_in*, char*, ...);

#endif /* LOGGING_H */
