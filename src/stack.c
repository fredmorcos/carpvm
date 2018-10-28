#include <carp/stack.h>
#include <stdint.h>
#include <inttypes.h>

/* Initialize  the  stack  with  pointer to  the  height  and  initial
 * height.
 */
bool carp_stack_init(struct carp_stack *s,
                     int64_t *height,
                     int64_t max_height) {
  int64_t *contents = NULL;

  assert(s != NULL);
  assert(height != NULL);
  assert(max_height > 0);

  s->max_height = max_height;
  s->height = height;

  if ((contents = calloc(max_height, sizeof(*contents))) == NULL) {
    return true;
  }

  s->contents = contents;

  return false;
}

/* Return true if the height is 0 (meaning the stack is empty) */
bool carp_stack_empty(struct carp_stack *s) {
  assert(s != NULL);
  return (*s->height) == 0;
}

/* Return true if  the height is the max height  (meaning the stack is
 * full).
 */
bool carp_stack_full(struct carp_stack *s) {
  assert(s != NULL);
  return (*s->height) == s->max_height;
}

/* Push value onto the stack. Return 0 if stack push succeeds */
bool carp_stack_push(struct carp_stack *s, int64_t i) {
  assert(s != NULL);

  if (carp_stack_full(s)) {
    int64_t new_height, *new_contents = NULL;

    /* give  stack  2n   +  1  its  existing   space,  hopefully  more
     * efficiently allocating
     */
    new_height = 2 * s->max_height + 1;

    if ((new_contents = realloc
         (s->contents, new_height * sizeof(int64_t))) == NULL) {
      return true;
    }

    /* if all is well, update pointer and capacity */
    s->max_height = new_height;
    s->contents = new_contents;
  }

  assert(!carp_stack_full(s));

  /* push the value and increase the height */
  s->contents[(*s->height)] = i;
  (*s->height)++;

  return false;
}

/* Pop the top of the stack into v. Return 0 if stack pop succeeds */
bool carp_stack_pop (struct carp_stack *s, int64_t *v) {
  assert(s != NULL);

  if (carp_stack_empty(s)) {
    return true;
  } else {
    (*s->height)--;
    *v = s->contents[(*s->height)];
    return false;
  }
}

/* Peek the top of the stack into v. Return 0 if the peek succeeds */
bool carp_stack_peek (struct carp_stack *s, int64_t *v) {
  assert(s != NULL);

  if (carp_stack_empty(s)) {
    return true;
  } else {
    *v = s->contents[(*s->height) - 1];
  }

  return false;
}

/* Print the contents of the stack onto stdout */
void carp_stack_print (struct carp_stack *s, FILE *fp) {
  assert(s != NULL);

  if (fp == NULL) {
    fp = stdout;
  }

  fprintf(fp, "[ ");

  for (int64_t i = 0; i < (*s->height); i++) {
    fprintf(fp, "%" PRId64 " ", s->contents[i]);
  }

  fprintf(fp, "]\n");
}

/* Clean up the stack memory */
void carp_stack_cleanup (struct carp_stack *s) {
  assert(s != NULL);

  free(s->contents);
  s->contents = NULL;
  s->max_height = 0;
  (*s->height) = 0;
}
