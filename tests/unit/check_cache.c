#include <check.h>

#include "../../include/cache.h"

/* private functions */
uint16_t _hash(char *buf, size_t len);

START_TEST (test_sum_returns_checksum)
{
	char		*buf_a = "AAA";
	char		*buf_b = "The quick brown fox...";

	fail_unless(195 == _hash(buf_a, strlen(buf_a)));
	/*
	 * Test the value returned is no larger that the cardinality
	 * of the cache.
	 */
	fail_unless(952 == _hash(buf_b, strlen(buf_b)));
}
END_TEST

START_TEST (test_add_to_cache)
{
	char		*key = "AAA";
	char		*value = "BBB";
	fail_unless(0 == cache_add(key, value));

	/*
	 * This simulates cache retrieval.
	 */
	fail_unless(0 == strcmp("BBB", CACHE.keys [195]->value));
}
END_TEST

START_TEST (test_update_cache)
{
	char		*key = "AAA";
	char		*value_a = "BBB";
	char		*value_b = "CCC";

	/*
	 * This simulates initialization.
	 */
	bzero(CACHE.keys, sizeof(CACHE.keys));

	fail_unless(0 == cache_add(key, value_a));
	fail_unless(0 == strcmp("BBB", CACHE.keys [195]->value));
	fail_unless(1 == cache_add(key, value_b));
	fail_unless(0 == strcmp("CCC", CACHE.keys [195]->value));
}
END_TEST

Suite *
cache_suite(void)
{
	Suite *s = suite_create("cache");

	TCase *tc_cache = tcase_create("cache");

	tcase_add_test(tc_cache, test_sum_returns_checksum);
	tcase_add_test(tc_cache, test_add_to_cache);
	tcase_add_test(tc_cache, test_update_cache);

	suite_add_tcase(s, tc_cache);

	return s;
}
