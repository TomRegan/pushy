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
 * @file  protocol_handler.c
 * @brief  functions to get and process http requests
 * @author  Tom Regan <code.tom.regan@gmail.com>
 */

#include "../include/protocol_handler.h"

size_t
_remaining_size(int max, char *buf)
{
	return (max - strnlen(buf, max));
}

int8_t
_start_message(char *msg_buf)
{
	char		*c;

	strncpy(msg_buf, "HTTP/1.1 404 Not Found\r\n", HTTP_RESPONSE_LEN);

	return 0; /* no error */
}

int8_t
_insert_content_length(char *msg_buf, char *msg_body)
{
	const size_t	MAX_LEN = _remaining_size(HTTP_HEAD_LEN, msg_buf);
	const char	*fmt_buf = "Content-Length: %zu\r\n";

	char		tmp_buf   [HTTP_HEAD_LEN + 1];

	if (!MAX_LEN > 0) {
		return -1; /* header too long */
	}

	snprintf(tmp_buf, sizeof(tmp_buf), fmt_buf,
			strnlen(msg_body, HTTP_RESPONSE_LEN));
	strncat(msg_buf, tmp_buf, HTTP_HEAD_LEN);

	return 0; /* no error */
}

int8_t
_finalise_message_body(char *msg_body, struct request *req, char *rsp_str)
{
	const char     *fmt_buf = "{\"request\":\"%s\",\"response\":\"%s\"}\r\n";

	snprintf(msg_body, HTTP_BODY_LEN, fmt_buf, req->uri, rsp_str);

	return 0; /* no error */
}

int8_t
_insert_date(char *msg_buf)
{
	const size_t	MAX_LEN = _remaining_size(HTTP_HEAD_LEN, msg_buf);
	const char	*fmt_buf = "Date: %s\r\n\r\n";

	time_t		rawtime;
	char		time_buffer[RFC1123_TIME_LEN + 1];
	char		tmp_buf   [MAX_LEN + 1];

	if (!MAX_LEN > 0) {
		return -1; /* header too long */
	}
	time(&rawtime);
	strftime(time_buffer, RFC1123_TIME_LEN, RFC1123_TIME, gmtime(&rawtime));
	snprintf(tmp_buf, MAX_LEN, fmt_buf, time_buffer);
	strncat(msg_buf, tmp_buf, MAX_LEN);

	return 0; /* no error */
}

int8_t
_finalise_message(char *msg_buf, char *msg_body)
{
	char		*c;

	strncat(msg_buf, msg_body, HTTP_RESPONSE_LEN);

	return 0; /* no error */
}

uint8_t
send_response(int peerfd, char *msg_buf, char *rsp_str, struct request *req)
{
	char		msg_body  [HTTP_BODY_LEN + 1];
	uint8_t		error = 0;

	_finalise_message_body(msg_body, req, rsp_str);

	_start_message(msg_buf);

	if (_insert_content_length(msg_buf, msg_body) == -1) {
		error |= 0x1 << 7;
	}
	strncat(msg_buf, "Connection: close\r\n", HTTP_HEAD_LEN);
	strncat(msg_buf, "Content-Type: application/json; charset=utf-8\r\n", HTTP_HEAD_LEN);
	strncat(msg_buf, "Server: Pushy/0.0.1.1\r\n", HTTP_HEAD_LEN);

	if (_insert_date(msg_buf) == -1) {
		error |= 0x1;
	}
	_finalise_message(msg_buf, msg_body);

	if (error) {
		/* it's some kind of server error */
		msg_body[0] = '\0';
	}
	write(peerfd, msg_buf, strnlen(msg_buf, HTTP_RESPONSE_LEN));

	return error;
}

uint8_t
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

int8_t
_get_request_uri(char *request_line, struct request *req)
{
	char		tmp_buf   [MAX_URI_LEN + 1];
	int		i = MAX_URI_LEN;

	while (*request_line != ' ' && i-- >= 0) {
		request_line++;
	}

	if (i < 0) {
		return -1; /* uri is too long */
	}
	/*
	 * Copying is just to preserve the request buffer.
	 */
	strncpy(tmp_buf, request_line, sizeof(tmp_buf));
	strncpy(req->uri, strtok(tmp_buf, " \n\r\t"), MAX_URI_LEN);

	return 0; /* no error */
}

int8_t
_get_protocol_version(char *request, struct request *req)
{
	int		i = 0;
	char		*substr;

	if ((substr = strstr(request, "HTTP")) == NULL
		|| strlen(substr) < 10) {
		return -1; /* no http verison */
	}

	req->http_version.major = atoi(&substr [5]);
	req->http_version.minor = atoi(&substr [7]);

	return 0; /* no error */
}

int8_t
_read_header(int peerfd, char * buf, struct request *req)
{
	req->method = _get_request_method(buf);
	(void) _get_request_uri(buf, req);
	(void) _get_protocol_version(buf, req);
	/* TODO: if POST read content length */

	return 0; /* no error */
}

int
read_request(int peerfd, struct request *req)
{
	int		nbytes;
	char		buf [REQUEST_BUFFER_LEN + 1];

	nbytes = recv(peerfd, buf, REQUEST_HEAD_LEN, 0);
	buf[nbytes] = '\0';

	if (nbytes) {
		/* TODO: return more granular errors form _read... */
		if ((_read_header(peerfd, buf, req)) == -1) {
			return EREADHEAD;
		}
		/* TODO: if POST and content length > 0 read body */
	} else {
		if (nbytes == 0) {
			return ECONNRST; /* connection reset */
		} else if (nbytes == -1) {
			return ESOCKERR; /* read error */
		} else {
			return EUNKNOWN;
		}
	}

	return nbytes; /* no error */
}
