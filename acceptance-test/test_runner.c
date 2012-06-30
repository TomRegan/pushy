#include <cgreen/cgreen.h>
#include "http_tests.c"

TestSuite *http_tests();

int
main(void)
{
  TestSuite *suite = create_test_suite();

  add_suite(suite, http_tests());

  return run_test_suite(suite, create_text_reporter());
}
