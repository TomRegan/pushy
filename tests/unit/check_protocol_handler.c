#include <stdio.h>
#include <check.h>
#include <stdint.h>
#include "../../include/protocol_handler.h"

/* private functions */
void _finalise_message_body(char*, struct request*, char*);
uint8_t _get_request_uri(char*, struct request*);
unsigned char _get_request_method(char *);

START_TEST (test_request_url_is_parsed)
{
	struct request	req;
	char			req_str_1[] = "GET /foo/zen HTTP/1.1\r\n";
	char			req_str_2[] = "GET /foo/happy HTTP/1.1\r\n";
	char			*uri;
	int			error = 0;

	error = _get_request_uri(req_str_1, &req);
	fail_unless(0 == strcmp(req.uri, "/foo/zen"));
	fail_unless(0 == error);
	bzero(&req, sizeof(struct request));
	_get_request_uri(req_str_2, &req);
	fail_unless(0 == strcmp(req.uri, "/foo/happy"));
	fail_unless(0 == error);
}
END_TEST

START_TEST (test_method_and_uri_are_returned)
{
  char request[] = "GET /foo/zen/happy.html HTTP/1.1\r\n";
  struct request r;

  bzero(&r, sizeof(struct request));
  r.method = _get_request_method(request);
  _get_request_uri(request, &r);
  fail_unless(MGET == r.method);
  fail_unless(0 == strcmp(r.uri, "/foo/zen/happy.html"));
}
END_TEST

START_TEST (test_get_request_sets_correct_flag)
{
  unsigned char method = 0;
  /* we want to test that the correct flag is set for a
   * given request. There are 8 HTTP request methods
   * although only GET, POST and PUT are immediately
   * important. Let's see how long this comment stays
   * up-to-date.
   */
  method = _get_request_method("GET / HTTP/1.1");
  fail_unless(method == MGET);
}
END_TEST

START_TEST (test_post_request_sets_correct_flag)
{
  unsigned char method = 0;
  /* we expect two things from this test: that the
   * correct flag is set agian, but also that the
   * previously set flag has been unset, ie. the
   * result of evaluating the GET request is not
   * persistant
   */
  method = _get_request_method("POST / HTTP/1.1");
  fail_unless(method == MPOST);
}
END_TEST

START_TEST (test_unknown_request_sets_correct_flag)
{
  unsigned char method = 0;
  /* the aim of this test is to ensure malformed
   * requests can be handled in a graceful way
   */
  method = _get_request_method("ZEN /HAPPY FOO/1.1");
  fail_unless(method == MUNKNOWN);

  /* We'll also take a good crack at fucking this
   * method up
   */
  method = _get_request_method("");
  fail_unless(method == MUNKNOWN);
}
END_TEST

START_TEST (test_empty_request_sets_correct_flag)
{
  unsigned char method = 0;
  /* We need an empty request to set a testable
   * condition
   */
  method = _get_request_method("");
  fail_unless(method == MUNKNOWN);
}
END_TEST

START_TEST (test_long_request_sets_correct_flag)
{
	char			buffer [MAX_URI_LEN*2];
	struct request	req;
	int i, j, error = 0;

	bzero(buffer, MAX_URI_LEN*2);

	for (i = 0, j = MAX_URI_LEN*2; i < MAX_URI_LEN; ++i, --j) {
		/*
		 * This will create a string with 2048 characters.
		 * The character in the 1024th place will be a null.
		 * The string will begin with as and following the
		 * null be filled with bs (I mean letter 'B's).
		 */
		buffer [i] = 'a';
		buffer [j] = 'b';
	}
	error = _get_request_uri(buffer, &req);

	/*
	 * 0b10000000 is the error code to uri-too-long
	 */
	fail_unless(error == 0x1 << 7);
}
END_TEST

START_TEST (test_non_ascii_sets_correct_flag)
{
  unsigned char method = 0;
  /* As this is the net it would be a good idea
   * to assume the worst: any ascii char might
   * come over the wire and should be handled
   * with decorum
   */
  char buf[3] = {(char)6, (char)1, '\0'};
  method = _get_request_method(buf);
  fail_unless(method == MUNKNOWN);
}
END_TEST

START_TEST (test_generate_json_body)
{
	char msg_body[1024];
	char* msg_content = "unit-test";
	struct request req = {
		.method = MGET,
		.uri = "/"
	};
	_finalise_message_body(msg_body, &req, msg_content);
	fail_unless(0 == strcmp("{\"/\":\"unit-test\"}\r\n", msg_body));
}
END_TEST
