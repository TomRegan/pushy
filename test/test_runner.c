#include <cgreen/cgreen.h>
#include "request_tests.c"

TestSuite *request_tests();

int
main(void)
{
  TestSuite *suite = create_test_suite();

  add_suite(suite, request_tests());

  return run_test_suite(suite, create_text_reporter());
}
