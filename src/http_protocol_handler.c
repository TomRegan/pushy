#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <arpa/inet.h>

#include "../include/buffers.h"
#include "../include/http_protocol_handler.h"
#include "../include/response_headers.h"

#define STRTOK strtok

void
_insert_json_body(char *msg_body, struct request *req, char *msg_content)
{
  char fmt_body[] = "{\"%s\":\"%s\"}";
  sprintf(msg_body, fmt_body, req->uri, msg_content);
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

char *
_get_request_uri(char *request_line)
{
  /* The buffer includes the request method which
   * we don't want; it's pointless to call strtok
   * just to get rid of the method so we'll skip
   * over it to the next space. HTTP says we will
   * *definitely* only have one space, *snigger*
   */
  while (*request_line != ' ') {

    request_line++;
  }

  /* TODO: rewrite so the original request is preserved */
  static char uri_buf[256];
  strncpy(uri_buf, request_line, sizeof(uri_buf));
  return STRTOK(uri_buf, " ");
}

void
_timestamp_message(char *msg_buf, char *header, char *msg_body)
{
  char time_buffer[RFC1123_TIME_LEN + 1];
  time_t rawtime;

  /* include RFC1123 formatted time string */
  time(&rawtime);
  strftime(time_buffer, RFC1123_TIME_LEN, RFC1123_TIME, gmtime(&rawtime));
  time_buffer[RFC1123_TIME_LEN] = '\0';
  sprintf(msg_buf, header, strlen(msg_body), time_buffer);
}

int
send_response(int peerfd, char *msg_buf, char *msg_content,
		struct request *req)
{

  char msg_body[HTTP_MESSAGE_SIZE];

  _insert_json_body(msg_body, req, msg_content);
  _timestamp_message(msg_buf, ERROR_NOT_FOUND, msg_body);
  strncat(msg_buf, msg_body, HTTP_MESSAGE_SIZE);

  write(peerfd, msg_buf, strlen(msg_buf));
  return close(peerfd);
}

/**
 * Co-oridinates receiving a request
 * @param req an empty request record
 * @param peerfd the connected peer form which to read
 * @return EXIT_SUCCESS on success or EXIT_FAILURE
 */
int
read_request(struct request *req, int peerfd)
{
  int req_length;
  char req_buffer[REQUEST_BUFFER_SIZE + 1];

  req_length = read(peerfd, req_buffer, REQUEST_BUFFER_SIZE);
  if (req_length) {

    req_buffer[req_length] = '\0';
  }

  req->method = _get_request_method(req_buffer);
  req->uri    = _get_request_uri(req_buffer);

  return (int) strnlen(req->uri, 256);
}
