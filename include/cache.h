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
 * @file  cache.h
 * @brief  in-memory data cache
 * @author  Tom Regan <code.tom.regan@gmail.com>
 */

#ifndef CACHE_H
#define CACHE_H 1

#include <semaphore.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* for Linux */
#include <fcntl.h>

#include "logging.h"

#define CACHE_KEYS          997
#define RECORD_KEY_LEN      255
#define RECORD_ENTRY_LEN    1023

struct record {
    char            key [RECORD_KEY_LEN + 1];
    char            value [RECORD_ENTRY_LEN + 1];
    struct record   *l;
    struct record   *r;
};

struct cache {
	size_t		size;
	struct record	*keys [CACHE_KEYS];
};

/**
 * Initializes the cache for use. Multiple calls will have undefined
 * behaviour.
 */
int8_t
cache_init(void);

/**
 * Inserts/updates the value for the given key.
 *
 * @param  Key
 * @param  Value
 *
 * @return  0 on successful insert, 1 on successful update, -1 on
 * failure
 */
int8_t
cache_add(char*, char*);

/**
 * Retrieves the value for the key (1) and places it into the buffer (2)
 *
 * @param  Key
 * @param  Buffer in which to store the value
 * @param  Size of the buffer passed as param 2
 *
 * @return  0 on success, -1 if not found
 */
int8_t
cache_rtrv(char*, char [], size_t);

/**
 * Removes the record for the given key.
 *
 * @param  Key
 *
 * @return  0 on success, -1 if not found
 */
int8_t
cache_rm(char*);

/**
 * Deletes all entries from the cache.
 *
 * @return  0 on success, -1 on failure
 */
int8_t
cache_clear(void);

/**
 * Get the number of records in the cache
 *
 * @return  number of records
 */
size_t
cache_size(void);

struct cache	CACHE;
sem_t		*CACHE_LOCK;

#endif /* CACHE_H */
