#include <check.h>
#include "check_request.c"

Suite *
request_suite(void)
{
	Suite *s = suite_create("request");

	TCase *tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_long_uri_gets_414_response);
	tcase_add_test(tc_core, test_request_url_is_parsed);
	tcase_add_test(tc_core, test_method_and_uri_are_returned);
	tcase_add_test(tc_core, test_get_request_sets_correct_flag);
	tcase_add_test(tc_core, test_post_request_sets_correct_flag);
	tcase_add_test(tc_core, test_unknown_request_sets_correct_flag);
	tcase_add_test(tc_core, test_empty_request_sets_correct_flag);
	tcase_add_test(tc_core, test_non_ascii_sets_correct_flag);

	suite_add_tcase(s, tc_core);

	return s;
}

int
main(void)
{
	int number_failed;
	Suite *s = request_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? 0 : 1;
}
