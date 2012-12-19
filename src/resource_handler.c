#include "../include/resource_handler.h"
#include "../include/protocol_handler.h"
#include "../include/cache.h"

uint16_t
service_request(struct request *req, char *rtrv_buf, const size_t len)
{

    if (req->method == MGET) {
        if (cache_rtrv(req->uri, rtrv_buf, len) == 0) {
            return ROK;
        } else {
            log_ln(DEBUG, "'%s' not found in cache\n", req->uri);
            // send to error_not_found handler
            return RNOTFOUND;
        }
    }

    return RINTERNAL;
}
