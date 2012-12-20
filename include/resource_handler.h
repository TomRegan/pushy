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
 * @file  resource_handler.h
 * @brief  functions to handle requests to the cache
 * @author  Tom Regan <code.tom.regan@gmail.com>
 */

#ifndef RESOURCE_H
#define RESOURCE_H 1

#include <stdlib.h>

#include "../include/protocol_handler.h"
#include "../include/resource_handler.h"
#include "../include/cache.h"

/**
 * service_request
 *
 * provides requess with access to resources
 *
 * @param  req  a populated request record
 * @param  rtrv_buf  a buffer to hold retrieved resources
 * @param  rtrv_buf_len  the size of the rtrv buffer
 *
 * @returns  size_t  size of the rtrv buffer
 */
uint16_t service_request(struct request*, char*, const size_t);

#endif
