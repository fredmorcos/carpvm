/**
 * `test.h` - libtest
 */

#ifndef TEST_H
#define TEST_H

#include <stdbool.h>

/**
 * Checks the test condition and prints the according message.
 *
 * @param `res`  Test or assertion result.
 * @param `good` String to print in case of success.
 * @param `bad`  Format string to print in case of failure.
 *
 * @return  Whether the  case succeeded  or failed.  Basically returns
 * `res` so that the caller need not re-run the test-case.
 */
bool test(const bool res, const char *const good, const char *const bad, ...);

#endif
