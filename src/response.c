#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../include/response_headers.h"
#include "../include/buffers.h"
#include "../include/request.h"
#include "../include/response.h"

/**
 * Sends a 'request-uri too long' (414) response when the request uri
 * is longer than the maximum supported by the server
 * @param msg_body the message body buffer
 * @param struct request * the original request
 */
void
respond_uritoolong(char * msg_body, struct request *req)
{
  char fmt_body[] = "{\"%s\":\"uri too long\"}";
  sprintf(msg_body, fmt_body, req->uri);
}

/**
 * Sends a 'not-found' (404) response when the resource requested could
 * not be found
 * @param msg_body the message body buffer
 * @param struct request * the original request
 */
void
respond_notfound(char * msg_body, struct request *req)
{
  char fmt_body[] = "{\"%s\":\"not-found\"}";
  sprintf(msg_body, fmt_body, req->uri);
}

void
_timestamp_message(char *message, char *header, char *msg_body)
{
  char time_buffer[RFC1123_TIME_LEN + 1];
  time_t rawtime;

  /* include RFC1123 formatted time string */
  time(&rawtime);
  strftime(time_buffer, RFC1123_TIME_LEN, RFC1123_TIME, gmtime(&rawtime));
  time_buffer[RFC1123_TIME_LEN] = '\0';
  sprintf(message, header, strlen(msg_body), time_buffer);
  strcat(message, msg_body);
}

/**
 * Builds a response string and sends it to a peer
 * @param peerfd descriptor of the peer
 * @param msg_buffer the http content to send
 * @return >=0 on success, -1 on error
 */
int
send_response(int peerfd, char *message, struct request *req,
		void (*response_fp)(char *, struct request *) )
{

  char msg_body[HTTP_MESSAGE_SIZE];
  (*response_fp)(msg_body, req);

  /* TODO header is determined by response type */
  _timestamp_message(message, ERROR_NOT_FOUND, msg_body);

  write(peerfd, message, strlen(message));
  return close(peerfd); /* do we actually want to close this connection? */
}
