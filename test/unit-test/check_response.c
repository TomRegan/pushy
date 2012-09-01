#include <check.h>
#include <request.h>
#include <response.h>

START_TEST (test_uritoolong_sets_correct_message)
{
  /* char fmt_body[] = "{\"%s\":\"uri too long\"}"; */
  /* sprintf(msg_body, fmt_body, req->uri); */
  fail("not implemented");
}
END_TEST

START_TEST (test_not_found_sets_correct_message)
{
  /* char fmt_body[] = "{\"%s\":\"not-found\"}"; */
  /* sprintf(msg_body, fmt_body, req->uri); */
  fail("not implemented");
}
END_TEST

START_TEST (test_long_uri_request_returns_414)
{
	fail("not implemented");
}
END_TEST
