/**
 * author      : Tom Regan <code.tom.regan@gmail.com>
 * description :
 * Acceptance test for httpd.
 *
 * The acceptance tests are intended to verify the runtime behaviour
 * of httpd, for instance that the correct response is given to
 * a particular request.
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

#include <check.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../../include/protocol_handler.h"
#include "../../include/buffers.h"

#define PORT 55080
#define LOCALADDR "127.0.0.1"
#define BUFFER_S 1024

int		sockfd;

static void
get_time(char *time_buffer)
{
	time_t		rawtime;
	time(&rawtime);
	strftime(time_buffer, RFC1123_TIME_LEN, RFC1123_TIME, gmtime(&rawtime));
	time_buffer[RFC1123_TIME_LEN] = '\0';
	return;
}

static void
set_up()
{
	struct sockaddr_in server;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		perror("socket");

	bzero(&server, sizeof(struct sockaddr));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr(LOCALADDR);

	if ((connect(sockfd, (struct sockaddr *)&server,
		     sizeof(struct sockaddr))) == -1)
		perror("connect");

	return;
}

START_TEST(test_long_uri_gets_414_response)
{
	fail("test not implemented");
}
END_TEST


START_TEST(test_error_not_found_gets_404_response)
{

	/* should return a valid http header and non-blank message */

	int			result;

	char		*request = "GET / HTTP/1.1";
	char		buf[BUFFER_S + 1];
	char		time_buffer[RFC1123_TIME_LEN + 1];
	char		response_template[HTTP_RESPONSE_LEN + 1] = "\
HTTP/1.1 404 Not Found\r\n\
Content-Length: 19\r\n\
Content-Type: text/html\r\n\
Server: Pushy/0.0.1.1\r\n\
Date: %s\r\n\
\r\n\
{\"/\":\"not-found\"}\r\n";
	char		expect_response[HTTP_RESPONSE_LEN + 1];
	size_t		expect_response_len;

	bzero(buf, BUFFER_S);
	write(sockfd, request, strlen(request));
	read(sockfd, buf, BUFFER_S);

	/*
	 * the response we expect is the message header including the time,
	 * i.e. (length of time buffer + length of response template),
	 * plus a java script object literal map containing the uri and
	 * the status "not-found".
	 */
	get_time(time_buffer);

	/*
	 * we need to modify expect_response with the correct time --
	 * predictably there's a case where this test will fail because the
	 * time has rolled over 1 second between the test generating the
	 * current date and the response being received from the server, so
	 * it would be polite to fix this, but based on a loop of tests I ran
	 * I estimate this is a problem about 1/500 times.
	 */
	sprintf(expect_response, response_template, time_buffer);
	expect_response_len = strlen(expect_response);

	/*
	 * strncmp: test the response received is identical to the expected
	 * string but ignore any fluff like html
	 */
	result = strncmp(buf, expect_response, expect_response_len);
	fail_unless(result == 0);
}
END_TEST

#define NREQS 4
START_TEST(test_malformed_request_uri_gets_error_bad_request)
{
	fail("test not implemented");
}
END_TEST

static void
tear_down()
{
	close(sockfd);
}
