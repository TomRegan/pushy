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

typedef enum {
    NONE,
    ERROR,
    INFO,
    FINE,
    FINER,
    FINEST,
    DEBUG,
    MEM_DEBUG
} LOG_LEVEL;

#define LEVEL DEBUG /* logging level */

#define LOG_BUFFER_LEN 255

void log_ln(LOG_LEVEL, char*, ...);
void log_conn(LOG_LEVEL, struct sockaddr_in*, char*, ...);

#endif /* LOGGING_H */
