/**
 * author      : Tom Regan <code.tom.regan@gmail.com>
 * description :
 * Routines that directly communicate with the client over a socket.
 *
 *
 * Copyright 2012 Tom Regan <code.tom.regan@gmail.com>
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
 **/

#include <arpa/inet.h>

#ifndef REQUEST_H
#define REQUEST_H 1

#define MAX_URI_LEN 1024

/* http methods */
#define MOPTIONS   1
#define MGET       2
#define MHEAD      4
#define MPOST      8
#define MPUT      16
#define MDELETE   32
#define MTRACE    64
#define MUNKNOWN 128

/* timestamp format */
#define RFC1123_TIME "%a, %d %b %Y %H:%M:%S GMT"
#define RFC1123_TIME_LEN 30

/* error status messages */
#define SNOTFOUND "not-found"
#define SURITOOLONG "uri too long"

struct
http_version
{
  char major;
  char minor;
};

struct
request
{
  unsigned char method;
  char *uri;
};

int read_request(struct request*, int);

/**
 * Builds a response string and sends it to a peer
 * @param peerfd descriptor of the peer
 * @param msg_buf a buffer that will hold the complete message
 * @param msg_content the message content
 * @param r an http request struct
 * @return >=0 on success, -1 on error
 */
int send_response(int, char*, char*, struct request*);

#endif /* REQUEST_H */
