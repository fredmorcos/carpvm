#include <carp/stack.h>
#include <test/test.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

int main () {
  bool status = 0;
  struct carp_stack s;
  int64_t esp = 0;
  int64_t val = 0;
  int64_t to_push = 1;
  int64_t initial_height = 1;
  int64_t num_pushes = 5;

  bool test_res = true;

  status = carp_stack_init(&s, &esp, initial_height);
  test_res &= test(status == 0,
                   "Make stack",
                   "Cannot make stack, status = %d", status);

  status = carp_stack_empty(&s);
  test_res &= test(status == 1,
                   "Stack empty",
                   "Stack is not empty, status = %d", status);

  status = carp_stack_peek(&s, &val);
  test_res &= test(status == 1,
                   "Peek empty stack",
                   "Should not be able to peek empty stack, status = %d",
                   status);

  status = carp_stack_pop(&s, &val);
  test_res &= test(status == 1,
                   "Pop empty stack",
                   "Should not be able to pop empty stack, status = %d",
                   status);

  status = 0;
  for (int i = 0; i < num_pushes; i++) {
    status += carp_stack_push(&s, to_push);
  }

  test_res &= test(status == 0,
                   "Push to stack",
                   "Could not push to stack, status = %d", status);
  test_res &= test(esp > initial_height, "Stack height growth",
                   "Stack height didn't grow, esp = %d, initial_height = %d",
                   esp, initial_height);

  status = carp_stack_peek(&s, &val);
  test_res &= test(status == 0,
                   "Peek non-empty stack",
                   "Should be able to peek non-empty stack, status = %d",
                   status);
  test_res &= test(val == to_push,
                   "Peek == last pushed value",
                   "Peeked value != last pushed value, val = %d, to_push = %d",
                   val, to_push);

  status = carp_stack_pop(&s, &val);
  test_res &= test(status == 0,
                   "Pop non-empty stack",
                   "Should be able to pop non-empty stack, status = %d",
                   status);
  test_res &= test(val == to_push,
                   "Pop == last pushed value",
                   "Popped value != last pushed value, val = %d, to_push = %d",
                   val, to_push);

  carp_stack_cleanup(&s);
  test_res &= test(s.contents == NULL,
                   "Contents are NULL after cleanup",
                   "Contents are not NULL after cleanup, s.contents = %p",
                   s.contents);

  return !test_res;
}
