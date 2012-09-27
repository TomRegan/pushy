#include <check.h>

#include "../../include/cache.h"
#include "../../include/logging.h"

#define BINARY	0x0
#define LIST	0x1

/* private functions */
uint16_t _hash(char *buf, size_t len);
struct record *_rm(char *, struct record *, struct record **);

/* globals */
struct record *a, *b, *c, *d, *e, *f, *EXCLUDE = NULL;

void
setup(void)
{
	if (a == NULL) a = malloc(sizeof(struct record));
	if (b == NULL) b = malloc(sizeof(struct record));
	if (c == NULL) c = malloc(sizeof(struct record));
	if (d == NULL) d = malloc(sizeof(struct record));
	if (e == NULL) e = malloc(sizeof(struct record));
	if (f == NULL) f = malloc(sizeof(struct record));
	write_log(TEST_DEBUG, "MEMORY ALLOCATION\n\ta:%p\n\tb:%p\n\tc:%p\n\td:%p\n\te:%p\n\tf:%p\n", a, b, c, d, e, f);
}

struct record *
node(struct record *r, char *key, char *value)
{
	strcpy(r->key, key);
	strcpy(r->value, value);
	r->l = NULL;
	r->r = NULL;
	return r;
}

void
binary_tree(int type)
{
	/*
	 *         f
	 *        /
	 *       b
	 *      / \
	 *     a   e
	 *        / \
	 *       c   d
	 *
	 */
	a = node(a, "AAA", "aaa");
	b = node(b, "BBB", "bbb");
	c = node(c, "CCC", "ccc");
	d = node(d, "DDD", "ddd");
	e = node(e, "EEE", "eee");
	f = node(f, "FFF", "fff");
	if (BINARY == type) {
		f->l = b;
		b->l = a;
		b->r = e;
		e->l = c;
		e->r = d;
	} else if (LIST == type) {
		a->r = b;
		b->r = c;
		c->r = d;
		d->r = e;
		e->r = f;
	}
}

void
test_node(struct record *r)
{
	EXCLUDE = r;
}

void
teardown(void)
{
	if (EXCLUDE != a) { (void) free(a); a = NULL; }
	if (EXCLUDE != b) { (void) free(b); b = NULL; }
	if (EXCLUDE != c) { (void) free(c); c = NULL; }
	if (EXCLUDE != d) { (void) free(d); d = NULL; }
	if (EXCLUDE != e) { (void) free(e); e = NULL; }
	if (EXCLUDE != f) { (void) free(f); f = NULL; }
	EXCLUDE = NULL;
}

START_TEST (test_search_and_remove)
{
	binary_tree(BINARY);
	test_node(e);

	write_log(TEST_DEBUG, "REMOVING EEE\n");
	fail_unless(f == _rm("EEE", f, NULL));
	fail_unless(b->r == d);
	fail_unless(d->l == c);
}
END_TEST

START_TEST (test_remove_root_node)
{
	struct record	*result;

	binary_tree(BINARY);
	test_node(f);

	write_log(TEST_DEBUG, "REMOVING FFF\n");
	fail_unless(b == _rm("FFF", f, NULL));
}
END_TEST

START_TEST (test_remove_terminal_node)
{
	binary_tree(BINARY);
	test_node(a);

	write_log(TEST_DEBUG, "REMOVING AAA\n");
	fail_unless(f == _rm("AAA", f, NULL));
	fail_unless(b->l == NULL);
}
END_TEST

START_TEST (test_remove_and_relink)
{
	binary_tree(BINARY);
	test_node(b);

	write_log(TEST_DEBUG, "REMOVING BBB\n");
	fail_unless(f == _rm("BBB", f, NULL));
	fail_unless(f->l == e);
	fail_unless(c->l == a);
}
END_TEST

START_TEST (test_remove_from_list)
{
	binary_tree(LIST);
	test_node(c);
	fail_unless(a == _rm("CCC", a, NULL));
	fail_unless(b->r == d);
}
END_TEST

Suite *
tree_suite(void)
{
	Suite *s = suite_create("cache-tree");

	TCase *tc_tree = tcase_create("cache-tree");

	tcase_add_checked_fixture (tc_tree, setup, teardown);
	tcase_add_test(tc_tree, test_search_and_remove);
	tcase_add_test(tc_tree, test_remove_root_node);
	tcase_add_test(tc_tree, test_remove_terminal_node);
	tcase_add_test(tc_tree, test_remove_and_relink);
	tcase_add_test(tc_tree, test_remove_from_list);

	suite_add_tcase(s, tc_tree);

	return s;
}
