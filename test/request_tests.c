#include <cgreen/cgreen.h>
#include "../include/request.h"

#include <strings.h>
#include <string.h>

static void
test_get_request_sets_correct_flag()
{
  unsigned char method = 0;
  /* we want to test that the correct flag is set for a
   * given request. There are 8 HTTP request methods
   * although only GET, POST and PUT are immediately
   * important. Let's see how long this comment stays
   * up-to-date.
   */
  method = get_request_method("GET / HTTP/1.1");
  assert_equal(method, MGET);
}

static void
test_post_request_sets_correct_flag()
{
  unsigned char method = 0;
  /* we expect two things from this test: that the
   * correct flag is set agian, but also that the
   * previously set flag has been unset, ie. the
   * result of evaluating the GET request is not
   * persistant
   */
  method = get_request_method("POST / HTTP/1.1");
  assert_equal(method, MPOST);
}

static void
test_unknown_request_sets_correct_flag()
{
  unsigned char method = 0;
  /* the aim of this test is to ensure malformed
   * requests can be handled in a graceful way
   */
  method = get_request_method("ZEN /HAPPY FOO/1.1");
  assert_equal(method, MUNKNOWN); method = 0;

  /* We'll also take a good crack at fucking this
   * method up
   */
  method = get_request_method("");
  assert_equal(method, MUNKNOWN); method = 0;
}

static void
test_empty_request_sets_correct_flag()
{
  unsigned char method = 0;
  /* We need an empty request to set a testable
   * condition
   */
  method = get_request_method("");
  assert_equal(method, MUNKNOWN); method = 0;
}

static void
test_non_ascii_sets_correct_flag()
{
  unsigned char method = 0;
  /* As this is the net it would be a good idea
   * to assume the worst: any ascii char might
   * come over the wire and should be handled
   * with decorum
   */
  char buf[3] = {(char)6, (char)1, '\0'};
  method = get_request_method(buf);
  assert_equal(method, MUNKNOWN);
}

static void
test_request_url_is_parsed()
{
  char req_str_1[] = "GET /foo/zen HTTP/1.1\r\n";
  char req_str_2[] = "GET /foo/happy HTTP/1.1\r\n";
  char *uri;

  uri = get_request_uri(req_str_1);
  assert_equal(0, strcmp(uri, "/foo/zen"));
  uri = get_request_uri(req_str_2);
  assert_equal(0, strcmp(uri, "/foo/happy"));
}

static void
test_method_and_uri_are_returned()
{
  char request[] = "GET /foo/zen/happy.html HTTP/1.1\r\n";
  struct request r;

  bzero(&r, sizeof(struct request));
  r.method = get_request_method(request);
  r.uri = get_request_uri(request);
  assert_equal(MGET, r.method);
  assert_equal(0, strcmp(r.uri, "/foo/zen/happy.html"));
}

static TestSuite
*request_tests()
{
  TestSuite *suite = create_test_suite();

  /* get_request_method test */
  add_test(suite, test_get_request_sets_correct_flag);
  add_test(suite, test_post_request_sets_correct_flag);
  add_test(suite, test_unknown_request_sets_correct_flag);
  add_test(suite, test_non_ascii_sets_correct_flag);

  /* get_request_uri tests */
  add_test(suite, test_request_url_is_parsed);

  /* reading a whole request */
  add_test(suite, test_method_and_uri_are_returned);

  return suite;
}
