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
 * @file  protocol_handler.c
 * @brief  functions to get and process http requests
 * @author  Tom Regan <code.tom.regan@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <arpa/inet.h>

#include "../include/buffers.h"
#include "../include/protocol_handler.h"


size_t
_remaining_size(int max, char *buf)
{
	return (max - strnlen(buf, max));
}

void
_start_message(char *msg_buf)
{
	msg_buf [0] = '\0';
	strncpy(msg_buf, "HTTP/1.1 404 Not Found\r\n", HTTP_RESPONSE_LEN);
}

int
_insert_content_length(char *msg_buf, char *msg_body)
{
	const size_t	MAX_LEN = _remaining_size(HTTP_HEAD_LEN, msg_buf);
	const char	*fmt_buf = "Content-Length: %zu\r\n";

	char		tmp_buf [HTTP_HEAD_LEN];

	if(!MAX_LEN > 0) {
		return -1; /* header too long */
	}

	snprintf(tmp_buf, sizeof(tmp_buf), fmt_buf, strnlen(msg_body, HTTP_RESPONSE_LEN));
	strncat(msg_buf, tmp_buf, HTTP_HEAD_LEN);

	return 0; /* no error */
}

int
_finalise_message_body(char *msg_body, struct request *req, char *rsp_str)
{
	const char	*fmt_buf = "{\"%s\":\"%s\"}\r\n";

	return snprintf(msg_body, HTTP_BODY_LEN, fmt_buf, req->uri, rsp_str);
}

int
_insert_date(char *msg_buf)
{
	const size_t	MAX_LEN = _remaining_size(HTTP_HEAD_LEN, msg_buf);
	const char		*fmt_buf = "Date: %s\r\n\r\n";

	time_t		rawtime;
	char		time_buffer [RFC1123_TIME_LEN + 1];
	char		tmp_buf [MAX_LEN];

	if (!MAX_LEN > 0) {
		return -1; /* header too long */
	}

	time(&rawtime);
	strftime(time_buffer, RFC1123_TIME_LEN, RFC1123_TIME, gmtime(&rawtime));
	snprintf(tmp_buf, MAX_LEN, fmt_buf, time_buffer);
	strncat(msg_buf, tmp_buf, MAX_LEN);

	return 0; /* no error */
}

void
_finalise_message(char *msg_buf, char *msg_body)
{
	strncat(msg_buf, msg_body, HTTP_RESPONSE_LEN);
}

unsigned char
_get_request_method(char *request_line)
{
	if (strncmp(request_line, "GET", 3) == 0) {

		return MGET;
	}

	if (strncmp(request_line, "POST", 4) == 0) {

		return MPOST;
	}

	return MUNKNOWN;
}

char*
_get_request_uri(char *request_line)
{
	/*
	 * The buffer includes the request method which we don't want; it's
	 * pointless to call strtok just to get rid of the method so we'll
	 * skip over it to the next space. HTTP says we will *definitely*
	 * only have one space, *snigger*
	 */
	while (*request_line != ' ') {

		request_line++;
	}

	/* TODO: rewrite so the original request is preserved */
	char		*uri_buf;

	uri_buf = strndup(request_line, MAX_URI_LEN);

	return strtok(uri_buf, " ");
}

int
send_response(int peerfd, char *msg_buf, char *rsp_str, struct request *req)
{
	char		msg_body [HTTP_BODY_LEN];
	int			error = 0;

	_finalise_message_body(msg_body, req, rsp_str);

	_start_message(msg_buf);
	error = _insert_content_length(msg_buf, msg_body);
	strncat(msg_buf, "Content-Type: text/html\r\n", HTTP_HEAD_LEN);
	strncat(msg_buf, "Server: Pushy/0.0.1-1\r\n", HTTP_HEAD_LEN);
	error = _insert_date(msg_buf);

	if (error < 0) {
		/* it's some kind of server error */
		msg_body [0] = '\0';
	}

	_finalise_message(msg_buf, msg_body);

	write(peerfd, msg_buf, strnlen(msg_buf, HTTP_RESPONSE_LEN));

	return close(peerfd);
}

int
read_request(struct request *req, int peerfd)
{
	char		req_buf [REQUEST_BUFFER_LEN + 1];
	int			req_length = read(peerfd, req_buf, REQUEST_BUFFER_LEN);
	puts(req_buf);

	if (req_length) {

		req_buf [req_length] = '\0';
	}

	req->method = _get_request_method(req_buf);
	strncpy(req->uri, _get_request_uri(req_buf), MAX_URI_LEN); 

	return (int) strnlen(req->uri, sizeof(req->uri));
}
