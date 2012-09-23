#include <check.h>

#include "include/check_protocol_handler.h"
#include "include/check_cache.h"


int
main(void)
{
	int		number_failed;

	number_failed = 0;

	Suite *request = request_suite();
	SRunner *request_runner = srunner_create(request);
	srunner_run_all(request_runner, CK_NORMAL);
	number_failed += srunner_ntests_failed(request_runner);
	srunner_free(request_runner);

	Suite *cache = cache_suite();
	SRunner *cache_runner = srunner_create(cache);
	srunner_run_all(cache_runner, CK_NORMAL);
	number_failed += srunner_ntests_failed(cache_runner);
	srunner_free(cache_runner);

	return (number_failed == 0) ? 0 : 1;
}
