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
_start_message(char *msg_buf, uint16_t response_code)
{
    char        *fmt_buf, *rsp_buf;

    fmt_buf = "HTTP/1.1 %s\r\n";

    switch (response_code) {
    case ROK:
        rsp_buf = "200 OK";
        break;
    case RNOTFOUND:
        rsp_buf = "404 Not Found";
        break;
    case RNOTIMPLEMENTED:
        rsp_buf = "501 Not Implemented";
        break;
    default:
        rsp_buf = "500 Internal Server Error";
    }

    snprintf(msg_buf, HTTP_HEAD_LEN, fmt_buf, rsp_buf);

    return 0; /* no error */
}

int8_t
_insert_content_length(char *msg_buf, char *msg_body)
{
    const size_t    MAX_LEN = _remaining_size(HTTP_HEAD_LEN, msg_buf);
    const char      *FMT_BUF = "Content-Length: %zu\r\n";

    char            tmp_buf [HTTP_HEAD_LEN + 1];

    if (!MAX_LEN > 0) {
        return -1; /* header too long */
    }

    snprintf(tmp_buf, sizeof(tmp_buf), FMT_BUF,
             strnlen(msg_body, HTTP_RESPONSE_LEN));
    strncat(msg_buf, tmp_buf, HTTP_HEAD_LEN);

    return 0; /* no error */
}

void
_write_error_status(char *rsp_str, char *status)
{
    size_t          offset;

    /* Rational for offset:
     *
     * from snprintf man page:
     *
     * "The snprintf() and vsnprintf() functions will write at most n-1 of the
     * characters printed into the output string (the n'th character then gets
     * the terminating `\0');"
     *
     * So:
     *   2 chars for the quotes in format
     *   1 char for '\0'
     */
    offset = 3;
    snprintf(rsp_str, strlen(status) + offset, "\"%s\"", status);
}

int8_t
_finalise_message_body(char *msg_body, struct request *req, char *rtrv_buf, uint16_t response_code)
{
    const char      *FMT_BUF;
    char            *rsp_str, tmp_rsp_str [HTTP_BODY_LEN + 1];

    FMT_BUF = "{\"request\":\"%s\",\"response\":%s}\r\n";

    if (response_code == ROK) {
        rsp_str = rtrv_buf;
    } else {
        switch (response_code) {
        case RNOTFOUND:
            _write_error_status(tmp_rsp_str, SNOTFOUND);
            break;
        case RNOTIMPLEMENTED:
            _write_error_status(tmp_rsp_str, SNOTIMPLEMENTED);
            break;
        default:
            _write_error_status(tmp_rsp_str, SINTERNAL);
        }
        rsp_str = tmp_rsp_str;
    }

    snprintf(msg_body, HTTP_BODY_LEN, FMT_BUF, req->uri, rsp_str);

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
	strncat(msg_buf, msg_body, HTTP_RESPONSE_LEN);

	return 0; /* no error */
}

uint8_t
send_response(int peerfd, char *msg_buf, char *rtrv_buffer, uint16_t response_code, struct request *req)
{
	char		msg_body  [HTTP_BODY_LEN + 1];
	uint8_t		error = 0;

    _finalise_message_body(msg_body, req, rtrv_buffer, response_code);

    _start_message(msg_buf, response_code);

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
    uint8_t         method;

    method = MUNKNOWN;

    if (strncmp(request_line, "GET", 3) == 0) {
        method = MGET;
    }
    if (strncmp(request_line, "POST", 4) == 0) {
        method = MPOST;
    }
    if (strncmp(request_line, "PUT", 3) == 0) {
        method = MPUT;
    }

    return method;
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
    int             i = 0;
    char            *substr;

    if ((substr = strstr(request, "HTTP")) == NULL
            || strnlen(substr, HTTP_HEAD_LEN) < 10) {
        return -1; /* no http verison */
    }

    req->http_version.major = atoi(&substr [5]);
    req->http_version.minor = atoi(&substr [7]);

    return 0; /* no error */
}

int8_t
_get_content_type(char *request, struct request *req)
{
    return 0;
}

int8_t
_get_content_length(char *request, struct request *req)
{
    size_t          token_len, start, end;
    char            *substr, *token, result [LINE_BUF_LEN + 1];

    token = "Content-Length:";
    token_len = 15;

    /* !!!
     * Don't replace this with strnstrn!
     * strnstrn is not portable
     */
    substr = strstr(request, token);
    substr += token_len;

    start = 0;
    for (end = 0; substr[end] != '\r'; ++end) {
        if (substr [end] == ' ') {
            ++start;
        }
    }
    /* FIXME
     * copies too much data. should only copy the header field/value
     * e.g. strncpy(result, substr + start, end - substr);
     * needs UT.
     */
    strncpy(result, substr + start, LINE_BUF_LEN);
    result [end - start] = '\0';
    req->content_len = atoi(result);

    return 0;
}

int8_t
_read_header(int peerfd, char * buf, struct request *req)
{
    int8_t         error;

    error = 0; /* no error */

    if ((req->method = _get_request_method(buf)) == MPOST) {
        error += _get_content_length(buf, req);
        error += _get_content_type(buf, req);
    }
    (void) _get_request_uri(buf, req);
    (void) _get_protocol_version(buf, req);

    return error;
}

int
read_request(int peerfd, struct request *req)
{
    int         nbytes;
    char        buf [REQUEST_BUFFER_LEN + 1];

    nbytes = recv(peerfd, buf, REQUEST_HEAD_LEN, 0);
    buf[nbytes] = '\0';

    if (nbytes) {
        /* TODO: return more granular errors from _read... */
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
