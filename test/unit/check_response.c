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
  char resp_buf[128];
  struct request r;
  bzero(&r, sizeof(struct request));
  r.method = MGET;
  r.uri    = "/";
  respond_notfound(resp_buf, &r);
  fail_unless(0 == strcmp("{\"/\":\"not-found\"}", resp_buf));
}
END_TEST

START_TEST (test_long_uri_request_returns_414)
{
	fail("not implemented");
}
END_TEST
