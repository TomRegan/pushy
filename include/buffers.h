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
 * @file  buffers.h
 * @brief  sizes for buffers
 * @author  Tom Regan <code.tom.regan@gmail.com>
 */

#ifndef BUFFERS_H
#define BUFFERS_H 1

#define REQUEST_BUFFER_LEN  2047
#define HTTP_RESPONSE_LEN   2047
#define HTTP_BODY_LEN       1023
#define HTTP_HEAD_LEN       1023
#define REQUEST_BODY_LEN    1023
#define REQUEST_HEAD_LEN    1023
#define MAX_URI_LEN         2083  /* support.microsoft.com/kb/208427 */
#define LINE_BUF_LEN        511

#endif /* BUFFERS_H */
