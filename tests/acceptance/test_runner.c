#include <check.h>
#include "check_http.c"

Suite *
request_suite(void)
{
	Suite *s = suite_create("acceptance-tests");

	TCase *tc_core = tcase_create("Core");

	tcase_add_checked_fixture(tc_core, set_up, tear_down);
	/* tcase_add_test(tc_core, test_long_uri_gets_414_response); */
	tcase_add_test(tc_core, test_error_not_found_gets_404_response);
	/* TODO: investigate below test: failing */
	/*tcase_add_test(tc_core, test_malformed_request_uri_gets_error_bad_request);*/

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
