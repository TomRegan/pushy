#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../include/response_headers.h"
#include "../include/buffers.h"
#include "../include/request.h"
#include "../include/response.h"

void
_insert_json_body(char *msg_body, struct request *req, char *msg_content)
{
  char fmt_body[] = "{\"%s\":\"%s\"}";
  sprintf(msg_body, fmt_body, req->uri, msg_content);
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
