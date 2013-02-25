#include <stdio.h>
#include <check.h>
#include <stdint.h>
#include "../../include/protocol_handler.h"

/* private functions */
void _finalise_message_body(char*, struct request*, char*, uint16_t);
int8_t _get_request_uri(char*, struct request*);
unsigned char _get_request_method(char *);
int8_t _get_protocol_version(char*, struct request*);
int8_t _start_message(char*, uint16_t);
int8_t _get_content_length(char*, struct request*);
int8_t _read_body(char*, struct request*);
char* _trim_both(char*);

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

START_TEST (test_protocol_version_is_parsed)
{
	char		*request = "GET / HTTP/1.1\r\n";
	struct request	req;
	int		ret = 1;

	bzero(&req, sizeof(struct request));
	ret = _get_protocol_version(request, &req);

	fail_unless(1 == req.http_version.major);
	fail_unless(1 == req.http_version.minor);
	fail_unless(0 == ret);
}
END_TEST

START_TEST (test_protocol_version_returns_minus_one_on_error)
{
	char		*request = "GET / HAPPY/0.9\r\n", *request2, *request3, *request4;
	struct request	req;

	bzero(&req, sizeof(struct request));
	fail_unless(-1 == _get_protocol_version(request, &req));

	request2 = "FOOZEN/000.9\r\n";
	fail_unless(-1 == _get_protocol_version(request2, &req));

	request3 = "";
	fail_unless(-1 == _get_protocol_version(request3, &req));

	request4 = "GET / HTTP/1.\r\n";
	fail_unless(-1 == _get_protocol_version(request4, &req));
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
	fail_unless(-1 ==_get_request_uri(buffer, &req));
}
END_TEST

START_TEST (test_non_ascii_sets_correct_flag)
{
  unsigned char method = 0;
  char buf[3] = {(char)6, (char)1, '\0'};
  method = _get_request_method(buf);
  fail_unless(method == MUNKNOWN);
}
END_TEST

START_TEST (test_generate_json_body)
{
	char msg_body[1024];
    char* msg_content = "\"unit-test\"";
	struct request req = {
		.method = MGET,
		.uri = "/"
	};
    _finalise_message_body(msg_body, &req, msg_content, ROK);
	fail_unless(0 == strcmp("{\"request\":\"/\",\"response\":\"unit-test\"}\r\n", msg_body));
}
END_TEST

START_TEST(test_internal_error)
{
    char        msg_body[1024];
    char        *msg_content = "internal server error";

    struct request req = {
        .method = MGET,
        .uri = "/"
    };
    _finalise_message_body(msg_body, &req, msg_content, RINTERNAL);
    fail_unless(0 == strcmp("{\"request\":\"/\",\"response\":\"internal server error\"}\r\n", msg_body));
}
END_TEST

START_TEST (test_request_for_nx_resource_returns_404_header)
{
    char        msg_buf [HTTP_BODY_LEN + 1];
    char        *response = "HTTP/1.1 404 Not Found\r\n";

    _start_message(msg_buf, RNOTFOUND);

    fail_unless(0 == strncmp(msg_buf, response, strlen(response)));
}
END_TEST

START_TEST (test_request_for_existing_resource_returns_200_header)
{
    char        msg_buf [HTTP_BODY_LEN + 1];
    char        *response = "HTTP/1.1 200 OK\r\n";

    _start_message(msg_buf, ROK);

    fail_unless(0 == strncmp(msg_buf, response, strlen(response)));
}
END_TEST

START_TEST (test_request_with_head_returns_501_header)
{
    char        msg_buf [HTTP_BODY_LEN + 1];
    char        *response = "HTTP/1.1 501 Not Implemented\r\n";

    _start_message(msg_buf, RNOTIMPLEMENTED);

    fail_unless(0 == strncmp(msg_buf, response, strlen(response)));
}
END_TEST

START_TEST (test_put_request_sets_correct_flag)
{
    unsigned char method = 0;

    method = _get_request_method("PUT / HTTP/1.1");
    fail_unless(method == MPUT);
}
END_TEST

START_TEST (test_content_length_is_returned)
{
    char            *request;
    struct request  req;

    bzero(&req, sizeof(struct request));
    request = "Content-Length: 3\r\n";

    (void) _get_content_length(request, &req);

    fail_unless(3 == req.content_len);
}
END_TEST

START_TEST (test_read_body_stores_data)
{
    char            request [80] = "\r\n\r\ndata\r\n";
    char            request2 [80] = "some\r\n\r\nmore data \r\n";
    struct request  req;

    bzero (&req, sizeof (struct request));
    _read_body(request, &req);

    fail_unless(0 == strcmp("data", req.body->buf));

    bzero(&req, sizeof (struct request));
    _read_body(request2, &req);

    fail_unless(0 == strcmp("more data", req.body->buf));
}
END_TEST

START_TEST (test_trim_surround_data_removes_control_chars)
{
    char            *expected_result;
    char            data [80] = "\r\nstuff\r\n";

    expected_result = "stuff";
    fail_unless(0 == strncmp(expected_result, _trim_both(data), strlen(expected_result)));
}
END_TEST

START_TEST (test_trim_surround_data_handles_null_string)
{
    char            data [80] = "\0", *expected_result;

    expected_result = "";
    fail_unless(0 == strncmp(expected_result, _trim_both(data), strlen(expected_result)));
}
END_TEST

Suite *
request_suite(void)
{
    Suite *s = suite_create("protocol handler");

    TCase *tc_request = tcase_create("protocol_handler");

    tcase_add_test(tc_request, test_request_url_is_parsed);
    tcase_add_test(tc_request, test_method_and_uri_are_returned);
    tcase_add_test(tc_request, test_get_request_sets_correct_flag);
    tcase_add_test(tc_request, test_post_request_sets_correct_flag);
    tcase_add_test(tc_request, test_unknown_request_sets_correct_flag);
    tcase_add_test(tc_request, test_empty_request_sets_correct_flag);
    tcase_add_test(tc_request, test_long_request_sets_correct_flag);
    tcase_add_test(tc_request, test_non_ascii_sets_correct_flag);
    tcase_add_test(tc_request, test_generate_json_body);
    tcase_add_test(tc_request, test_protocol_version_is_parsed);
    tcase_add_test(tc_request, test_protocol_version_returns_minus_one_on_error);
    tcase_add_test(tc_request, test_request_for_nx_resource_returns_404_header);
    tcase_add_test(tc_request, test_request_for_existing_resource_returns_200_header);
    tcase_add_test(tc_request, test_request_with_head_returns_501_header);
    tcase_add_test(tc_request, test_put_request_sets_correct_flag);
    tcase_add_test(tc_request, test_content_length_is_returned);
    tcase_add_test(tc_request, test_internal_error);
    tcase_add_test(tc_request, test_read_body_stores_data);
    tcase_add_test(tc_request, test_trim_surround_data_removes_control_chars);
    tcase_add_test(tc_request, test_trim_surround_data_handles_null_string);

    suite_add_tcase(s, tc_request);

    return s;
}
