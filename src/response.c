#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../include/response_headers.h"
#include "../include/buffers.h"
#include "../include/response.h"

/**
 * Builds a response string and sends it to a peer
 * @param peerfd descriptor of the peer
 * @param msg_buffer the http content to send
 * @return >=0 on success, -1 on error
 */
int
send_response(int peerfd, char *message)
{

  char time_buffer[RFC1123_TIME_LEN + 1];
  char msg_body[] = "{\"/\":\"not-found\"}";
  time_t rawtime;

  /* include RFC1123 formatted time string */
  time(&rawtime);
  strftime(time_buffer, RFC1123_TIME_LEN, RFC1123_TIME, gmtime(&rawtime));
  time_buffer[RFC1123_TIME_LEN] = '\0';
  sprintf(message, ERROR_NOT_FOUND, strlen(msg_body), time_buffer);
  strcat(message, msg_body);

  write(peerfd, message, strlen(message));
  close(peerfd);  /* do we actually want to close this connection? */

  return EXIT_SUCCESS;
}
