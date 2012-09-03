#include <stdio.h>
#include <check.h>
#include <request.h>
#include <response.h>

void _insert_json_body(char*, struct request*, char*);

START_TEST (test_generate_json_body)
{
	char msg_body[1024];
	char* msg_content = "unit-test";
	struct request req = {
		.method = MGET,
		.uri = "/"
	};
	_insert_json_body(msg_body, &req, msg_content);
	fail_unless(0 == strcmp("{\"/\":\"unit-test\"}", msg_body));
}
END_TEST
