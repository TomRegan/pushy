#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <arpa/inet.h>

#include "../include/buffers.h"
#include "../include/request.h"

#define STRTOK strtok
#define LOG_IN printf
#define LOG    puts

unsigned char
get_request_method(char *request_line)
{
  if (strncmp(request_line, "GET", 3) == 0)
    return MGET;
  if (strncmp(request_line, "POST", 4) == 0)
    return MPOST;
  return MUNKNOWN;
}

char *
get_request_uri(char *request_line)
{
  /* TODO one of the marvels of TDD is that this approach
   * wastes an ass-load of memory (by chopping a string
   * in two) _and_ makes a mess of the request buffer
   * for everyone else, *tut*. I will write a replacement
   * tokenizer that returns a copy of the matching
   * token, but this is currently "good enough"(tm)
   */
  /* The buffer includes the request method which
   * we don't want; it's pointless to call strtok
   * just to get rid of the method so we'll skip
   * over it to the next space. HTTP says we will
   * *definitely* only have one space, *snigger*
   */
  while (*request_line != ' ') request_line++;
  return STRTOK(request_line, " ");
}

int
read_request(struct request *req, int peerfd, struct sockaddr_in *peer_addr)
{
  int req_length;
  char req_buffer[REQUEST_BUFFER_SIZE + 1];

  req_length = read(peerfd, req_buffer, REQUEST_BUFFER_SIZE);
  if (req_length) {
    req_buffer[req_length] = '\0';
    //LOG_IN("\n>>> %s\n\n", inet_ntoa(peer_addr->sin_addr));
    /* LOG(req_buffer); */
  }
  req->method = get_request_method(req_buffer);
  req->uri    = get_request_uri(req_buffer);
  printf("got %i request for %s\n", req->method,req->uri);
  return EXIT_SUCCESS;
}
