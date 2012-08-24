#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <arpa/inet.h>

#include "../include/buffers.h"
#include "../include/request.h"

#define STRTOK strtok

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

/**
 * Co-oridinates receiving a request
 * @param req an empty request record
 * @param peerfd the connected peer form which to read
 * @return >=0 on success or -1 on error
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

  return EXIT_SUCCESS;
}
