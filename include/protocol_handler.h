/* Copyright 2012 Tom Regan <code.tom.regan@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*
 * @file  protocol_handler.h
 * @brief  functions to get and process http requests
 * @author  Tom Regan <code.tom.regan@gmail.com>
 */

#ifndef REQUEST_H
#define REQUEST_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include "buffers.h"
#include "logging.h"

/* http methods */
#define MOPTIONS	0x01
#define MGET		0x02
#define MHEAD		0x04
#define MPOST		0x08
#define MPUT		0x10
#define MDELETE		0x20
#define MTRACE		0x40
#define MUNKNOWN	0x80

/* timestamp format */
#define RFC1123_TIME "%a, %d %b %Y %H:%M:%S GMT"
#define RFC1123_TIME_LEN 30

/* error status messages */
#define SNOTFOUND "not-found"
#define SURITOOLONG "uri too long"

struct
http_version
{
	char		major;
	char		minor;
};

struct
request
{
	struct http_version	http_version;
	uint8_t		method;
	char		uri [MAX_URI_LEN];
	char		header [REQUEST_HEAD_LEN];
	char		body [REQUEST_BODY_LEN];
};

/**
 * Co-oridinates receiving a request
 * @param req an empty request record
 * @param peerfd the connected peer form which to read
 * @return The length of the uri up to (MAX_URI_LEN)
 */
int read_request(struct request*, int);

/**
 * Builds a response string and sends it to a peer
 * @param peerfd descriptor of the peer
 * @param msg_buf a buffer that will hold the complete message
 * @param msg_content the message content
 * @param r an http request struct
 * @return 0 on success, error status on error
 */
uint8_t send_response(int, char*, char*, struct request*);

#endif /* REQUEST_H */
