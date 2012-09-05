#include <check.h>

#include "check_protocol_handler.c"

Suite *
request_suite(void)
{
	Suite *s = suite_create("Test Runner");

	TCase *tc_request = tcase_create("protocol_handler");

	tcase_add_test(tc_request, test_request_url_is_parsed);
	tcase_add_test(tc_request, test_method_and_uri_are_returned);
	tcase_add_test(tc_request, test_get_request_sets_correct_flag);
	tcase_add_test(tc_request, test_post_request_sets_correct_flag);
	tcase_add_test(tc_request, test_unknown_request_sets_correct_flag);
	tcase_add_test(tc_request, test_empty_request_sets_correct_flag);
	tcase_add_test(tc_request, test_non_ascii_sets_correct_flag);
	tcase_add_test(tc_request, test_generate_json_body);

	suite_add_tcase(s, tc_request);

	return s;
}

int
main(void)
{
	int number_failed;
	Suite *s = request_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? 0 : 1;
}
