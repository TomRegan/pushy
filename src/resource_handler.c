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
 * @file  resource_handler.c
 * @brief  functions to handle requests to the cache
 * @author  Tom Regan <code.tom.regan@gmail.com>
 */

#include "../include/resource_handler.h"

uint16_t
service_request(struct request *req, char *rtrv_buf, const size_t len)
{
    if (req != NULL) {
        if (req->method == MGET) {
            if (cache_rtrv(req->uri, rtrv_buf, len) == 0) {
                return ROK;
            } else {
                log_ln(DEBUG, "'%s' not found in cache\n", req->uri);
                return RNOTFOUND;
            }
            /* FIXME
             * The code for POST seems to be failing in its
             * call to cache_add. I'm disabling this fttb.
             */
            /*
              } else if (req->method == MPOST && req->body->buf != NULL) {
              success = cache_add(req->uri, req->body->buf);
              if (success == 0) {
              (void) strcpy(rtrv_buf, "\"created\"");
              return ROK;
              } else if (success == 1) {
              (void) strcpy(rtrv_buf, "\"updated\"");
              return ROK;
              } else {
              return RINTERNAL;
              }
            */
        } else if (req->method == MUNKNOWN || req->method == MPOST) {
            log_ln(ERROR, "request method not implemented: 0x%x\n", req->method);
            return RNOTIMPLEMENTED;
        }
    }
    return RINTERNAL;
}
