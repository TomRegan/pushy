#include <cgreen/cgreen.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../include/request.h"
#include "../include/response.h"
#include "../include/buffers.h"

#define PORT 55080
#define LOCALADDR "127.0.0.1"
#define BUFFER_S 1024

int sockfd;

static void
get_time(char *time_buffer)
{
  time_t rawtime;
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
  server.sin_family      = AF_INET;
  server.sin_port        = htons(PORT);
  server.sin_addr.s_addr = inet_addr(LOCALADDR);

  if((connect(sockfd, (struct sockaddr *)&server,
	      sizeof (struct sockaddr))) == -1)
    perror("connect");

  return;
}

static void
test_error_not_found_sends_valid_http_header_and_non_blank_message()
{
  int result;

  char *header = "GET / HTTP/1.1";
  char buf[BUFFER_S];
  char time_buffer[RFC1123_TIME_LEN + 1];
  char response_template[HTTP_MESSAGE_SIZE] ="HTTP/1.1 404 Not Found\
\r\nContent-Length: %zu\
\r\nContent-Type: text/html\
\r\nServer: Pushy/0.0.1-1\
\r\nDate: %s\
\r\n\r\n";
  char expect_response[HTTP_MESSAGE_SIZE];
  size_t expect_response_len, response_len;

  bzero(buf, BUFFER_S);
  write(sockfd, header, strlen(header));
  read(sockfd, buf, BUFFER_S);
  
  /* the response we expect is the message header includeing the time, ie
   *      (length of time buffer + length of response template)
   * the +2 is to account for the fact that there is a substitution in the
   * response template; %s is replaced with the date leaving two fewer
   * characters than the length of both buffers combined
   */
  get_time(time_buffer); 
  response_len = strlen(buf)-(strlen(response_template)+strlen(time_buffer))+2;

  /* we need to modufy expect_response with the correct time
   * --
   * predictably there's a case where this test will fail
   * because the time has rolled over 1 second between the
   * test generating the current date and the response being
   * received from the server, so it would be polite to fix this,
   * but based on a loop of tests I ran I estimate this is a problem 
   * about 1/500 times.
   */
  sprintf(expect_response, response_template, response_len, time_buffer);
  expect_response_len = strlen(expect_response);

  /* strncmp: test the response received is identical to the expected string 
   * but ignore any fluff like html
   */
  result = strncmp(buf, expect_response, expect_response_len);
  assert_equal(result, 0);

  return;
}

#define NREQS 4
static void
test_malformed_request_uri_gets_error_bad_request()
{
  char *requests[NREQS] = {"GET // HTTP/1.1",
		       "POST / foo HTTP/1.1",
		       "PUT foo HTTP/1.1",
		       "HEAD /foo/bar /baz HTTP/1.1"};
  char response_template[HTTP_MESSAGE_SIZE] ="HTTP/1.1 400 Bad Request\r\n\
Content-Length: %zu\r\n\
Content-Type: text/html\r\n\
Server: Pushy/0.0.1-1\r\n\
Date: %s\r\n\
\r\n\r\n";
  char expect_response[HTTP_MESSAGE_SIZE];
  char time_buffer[RFC1123_TIME_LEN + 1];
  char buf[BUFFER_S];
  size_t response_len, i;
  int result;

  /* this is being debuggered, presently it doesn't work */
  for (i = 0; i < NREQS; ++i) {
    puts(requests[i]);
    printf("%zu\n", strlen(requests[i]));
    printf("l1: test %zu\n", i);
    bzero(buf, BUFFER_S);
    bzero(time_buffer, RFC1123_TIME_LEN + 1);
    bzero(expect_response, HTTP_MESSAGE_SIZE);
    puts("l2");    get_time(time_buffer);
    puts("l3");    response_len = strlen(buf)-(strlen(response_template)+strlen(time_buffer)) + 2;
    puts("l4");    sprintf(expect_response, response_template, response_len, time_buffer);
    puts("l5");    write(sockfd, requests[i], strlen(requests[i]));
    puts("l6");    read(sockfd, buf, BUFFER_S);
    puts("l7");    sleep(5);
    puts("l8");    result = strncmp(buf, expect_response, strlen(expect_response));
    puts("l9");    assert_equal(result, 0);
  }


  return;
}

static void
tear_down()
{
  close(sockfd);
}

static TestSuite 
*http_tests()
{
  TestSuite *suite = create_test_suite();

  setup(suite, set_up);
  teardown(suite, tear_down);

  add_test(suite, test_error_not_found_sends_valid_http_header_and_non_blank_message);

  /* tests which are in development, or are rubbish */
  /* add_test(suite, test_malformed_request_uri_gets_error_bad_request); */
 
  return suite;
}
