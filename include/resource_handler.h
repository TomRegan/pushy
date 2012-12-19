#ifndef RESOURCE_H
#define RESOURCE_H 1

#include <stdlib.h>
#include "../include/protocol_handler.h"

/**
 * service_request
 *
 * provides requess with access to resources
 *
 * @param  req  a populated request record
 * @param  rtrv_buf  a buffer to hold retrieved resources
 * @param  rtrv_buf_len  the size of the rtrv buffer
 *
 * @returns  size_t  size of the rtrv buffer
 */
uint16_t service_request(struct request*, char*, const size_t);

#endif
