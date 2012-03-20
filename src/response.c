#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/response_headers.h"
#include "../include/buffers.h"
#include "../include/response.h"

#define HT_ERROR_DOCS_BASE "../docs/"

typedef short int sflags_t;

int
load_external_resource(char *buf)
{
  FILE *fp;

  if ((fp = fopen(HT_ERROR_DOCS_BASE"ERROR_NOT_FOUND.html", "r")) == NULL)
    perror("fopen");
  else
    fread(buf, HTTP_MESSAGE_SIZE, 1, fp);
  return EXIT_SUCCESS;
}

int
handle_response(char *message)
{

  char time_buffer[RFC1123_TIME_LEN + 1];
  char msg_body[HTTP_MESSAGE_SIZE];
  time_t rawtime;

  /* eventually this will be refactored, html will be loaded into buffers
   * on startup and there will be an appropriate response to the request
   * at the moment we respond to everything with not found
   */
  bzero(msg_body, HTTP_MESSAGE_SIZE);
  load_external_resource(msg_body);

  time(&rawtime);
  strftime(time_buffer, RFC1123_TIME_LEN, RFC1123_TIME, gmtime(&rawtime));
  time_buffer[RFC1123_TIME_LEN] = '\0';

  /* here we combine the correct reponse with the time */
  sprintf(message, ERROR_NOT_FOUND, strlen(msg_body), time_buffer);
  strcat(message, msg_body);

  return EXIT_SUCCESS;
}
