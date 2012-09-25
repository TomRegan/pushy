#include <check.h>

#include "../../include/cache.h"
#include "../../include/logging.h"

/* private functions */
uint16_t _hash(char *buf, size_t len);
struct record *_rm(char *, struct record *, struct record **);

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

	cache_init();
	fail_unless(0 == cache_add(key, value));

	/*
	 * This simulates cache retrieval.
	 */
	fail_unless(0 == strcmp("BBB", CACHE.keys [195]->value));
	cache_clear();
	fail_unless(0 == cache_size());
}
END_TEST

START_TEST (test_multiple_adds_to_cache)
{
	cache_init();
	fail_unless(0 == cache_size());
	fail_unless(0 == cache_add("AAA", "aaa"));
	fail_unless(1 == cache_size());
	fail_unless(0 == cache_add("BBB", "bbb"));
	fail_unless(2 == cache_size());
	fail_unless(0 == cache_add("CCC", "ccc"));
	fail_unless(3 == cache_size());
	cache_clear();
	fail_unless(0 == cache_size());
}
END_TEST

START_TEST (test_update_cache)
{
	char		*key = "AAA";
	char		*value_a = "BBB";
	char		*value_b = "CCC";


	cache_init();
	fail_unless(0 == cache_size());

	fail_unless(0 == cache_add(key, value_a));
	fail_unless(1 == cache_size());
	fail_unless(0 == strcmp("BBB", CACHE.keys [195]->value));
	fail_unless(1 == cache_add(key, value_b));
	fail_unless(1 == cache_size());
	fail_unless(0 == strcmp("CCC", CACHE.keys [195]->value));
	cache_clear();
	fail_unless(0 == cache_size());
}
END_TEST

START_TEST (test_remove_from_cache)
{
	struct record *a, *b, *c, *d, *e, *f;
	a = malloc(sizeof(struct record));
	b = malloc(sizeof(struct record));
	c = malloc(sizeof(struct record));
	d = malloc(sizeof(struct record));
	e = malloc(sizeof(struct record));
	f = malloc(sizeof(struct record));
	write_log(TEST_DEBUG, "BINARY TREE NODE REMOVAL TEST: MEMORY ALLOCATION");
	write_log(TEST_DEBUG, "================================================");
	write_log(TEST_DEBUG, "\n\ta:%p\n\tb:%p\n\tc:%p\n\td:%p\n\te:%p\n\tf:%p\n", a, b, c, d, e, f);
	strcpy(a->key, "AAA");
	strcpy(a->value, "aaa");
	a->l = NULL;
	a->r = NULL;

	strcpy(b->key, "BBB");
	strcpy(b->value, "bbb");
	b->l = NULL;
	b->r = NULL;

	strcpy(c->key, "CCC");
	strcpy(c->value, "ccc");
	c->l = NULL;
	c->r = NULL;

	strcpy(d->key, "DDD");
	strcpy(d->value, "ddd");
	d->l = NULL;
	d->r = NULL;

	strcpy(e->key, "EEE");
	strcpy(e->value, "eee");
	e->l = NULL;
	e->r = NULL;

	strcpy(f->key, "FFF");
	strcpy(f->value, "fff");
	f->l = NULL;
	f->r = NULL;

	a->l = b;
	b->l = c;
	b->r = d;
	d->l = e;
	e->l = f;

	/*
	 * Call delete with head pointer (a), searching for b.
	 */
	_rm("BBB", a, NULL);
	fail_unless(a->l == d);
	fail_unless(d->l == e);
	fail_unless(e->l == f);
	fail_unless(f->l == c);
	free(a); free(c); free(d); free(e); free(f);
}
END_TEST

START_TEST (test_remove_from_cache_if_tree_only_has_right_nodes)
{
	struct record *a, *b, *c, *d;
	a = malloc(sizeof(struct record));
	b = malloc(sizeof(struct record));
	c = malloc(sizeof(struct record));
	d = malloc(sizeof(struct record));
	write_log(TEST_DEBUG, "BINARY TREE NODE REMOVAL TEST: MEMORY ALLOCATION");
	write_log(TEST_DEBUG, "================================================");
	write_log(TEST_DEBUG, "\n\ta:%p\n\tb:%p\n\tc:%p\n\td:%p\n", a, b, c, d);
	strcpy(a->key, "AAA");
	strcpy(a->value, "aaa");
	a->l = NULL;
	a->r = NULL;

	strcpy(b->key, "BBB");
	strcpy(b->value, "bbb");
	b->l = NULL;
	b->r = NULL;

	strcpy(c->key, "CCC");
	strcpy(c->value, "ccc");
	c->l = NULL;
	c->r = NULL;

	strcpy(d->key, "DDD");
	strcpy(d->value, "ddd");
	d->l = NULL;
	d->r = NULL;

	a->r = b;
	b->r = c;
	c->l = d;

	/*
	 * Call delete with head pointer (a), searching for b.
	 */
	_rm("BBB", a, NULL);
	fail_unless(a->r == c);
	fail_unless(c->l == d);
	free(a); free(c); free(d);
}
END_TEST
Suite *
cache_suite(void)
{
	Suite *s = suite_create("cache");

	TCase *tc_cache = tcase_create("cache");

	tcase_add_test(tc_cache, test_sum_returns_checksum);
	tcase_add_test(tc_cache, test_add_to_cache);
	tcase_add_test(tc_cache, test_multiple_adds_to_cache);
	tcase_add_test(tc_cache, test_update_cache);
	tcase_add_test(tc_cache, test_remove_from_cache);
	tcase_add_test(tc_cache, test_remove_from_cache_if_tree_only_has_right_nodes);

	suite_add_tcase(s, tc_cache);

	return s;
}
