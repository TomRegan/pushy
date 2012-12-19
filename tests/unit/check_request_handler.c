#include <check.h>
#include "../../include/protocol_handler.h"
#include "../../include/buffers.h"
#include "../../include/resource_handler.h"

START_TEST (test_service_reqeust_returns_zero)
{
    struct request  req;
    char            rtrv_buf [HTTP_BODY_LEN + 1];
    fail_unless(0 == service_request(&req, rtrv_buf, HTTP_BODY_LEN));
}
END_TEST

Suite *
resource_suite(void)
{
    Suite *s = suite_create("resource_handler");

    TCase *tc_resource = tcase_create("resource_handler");

    tcase_add_test(tc_resource, test_service_reqeust_returns_zero);

    suite_add_tcase(s, tc_resource);

    return s;
}
