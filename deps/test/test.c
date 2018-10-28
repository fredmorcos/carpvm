/**
 * `test.c` - libtest
 */

#include "test.h"
#include <console-colors/console-colors.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

bool test(const bool res, const char *const good, const char *const bad, ...) {
  if (res) {
    cc_fprintf(CC_FG_GREEN, stderr, "PASSED: ");
    fprintf(stderr, "%s\n", good);
  } else {
    va_list ap;

    cc_fprintf(CC_FG_RED, stderr, "FAILED: ");

    va_start(ap, bad);
    vfprintf(stderr, bad, ap);
    va_end(ap);

    fprintf(stderr, "\n");
  }

  return res;
}
