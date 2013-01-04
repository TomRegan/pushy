#include <check.h>
#include "../../include/protocol_handler.h"
#include "../../include/buffers.h"
#include "../../include/resource_handler.h"
#include "../../include/cache.h"

START_TEST (test_service_reqeust_returns_internal_error)
{
    /*
     * request with no method should return http 500
     */

    struct request  req;
    char            rtrv_buf [HTTP_BODY_LEN + 1];

    bzero (&req, sizeof (struct request));

    fail_unless(RINTERNAL == service_request(&req, rtrv_buf, HTTP_BODY_LEN));
}
END_TEST

START_TEST (test_service_request_returns_ok)
{
    struct request  req;
    char            rtrb_buf [HTTP_BODY_LEN + 1];

    cache_init();
    bzero (&req, sizeof (struct request));
    cache_add("/SYSTEM", "{\"request\":\"/SYSTEM\",\"version\":\"0.0.1.1\"}");

    req.method = MGET;
    strncpy(req.uri, "/SYSTEM", strlen("/SYSTEM"));

    fail_unless(ROK == service_request(&req, rtrb_buf, HTTP_BODY_LEN));
    fail_unless(0 < strlen(rtrb_buf));
}
END_TEST

START_TEST (test_service_request_returns_not_found)
{
    struct request  req;
    char            rtrv_buf [HTTP_BODY_LEN +1];
    char            *uri = "not/in/cache";

    cache_init();
    bzero(&req, sizeof (struct request));

    req.method = MGET;
    strncpy(req.uri, uri, strlen(uri));

    fail_unless(RNOTFOUND == service_request(&req, rtrv_buf, HTTP_BODY_LEN));
}
END_TEST

START_TEST (test_service_request_returns_not_implemented)
{
    struct request  req;
    char            rtrv_buf [HTTP_BODY_LEN +1];
    char            *uri = "/";

    cache_init();
    bzero(&req, sizeof (struct request));

    req.method = MPOST;
    strncpy(req.uri, uri, strlen(uri));

    fail_unless(RNOTIMPLEMENTED == service_request(&req, rtrv_buf, HTTP_BODY_LEN));
}
END_TEST

Suite *
resource_suite(void)
{
    Suite *s = suite_create("resource_handler");

    TCase *tc_resource = tcase_create("resource_handler");

    tcase_add_test(tc_resource, test_service_reqeust_returns_internal_error);
    tcase_add_test(tc_resource, test_service_request_returns_ok);
    tcase_add_test(tc_resource, test_service_request_returns_not_found);
    tcase_add_test(tc_resource, test_service_request_returns_not_implemented);

    suite_add_tcase(s, tc_resource);

    return s;
}
