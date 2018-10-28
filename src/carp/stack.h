#ifndef CARP_STACK_H
#define CARP_STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

struct carp_stack {
  int64_t  max_height;
  int64_t *height;
  int64_t *contents;
};

bool carp_stack_init(struct carp_stack *s,
                     int64_t *height,
                     int64_t max_height);
bool carp_stack_empty(struct carp_stack *s);
bool carp_stack_full(struct carp_stack *s);
bool carp_stack_push(struct carp_stack *s, int64_t i);
bool carp_stack_pop(struct carp_stack *s, int64_t *v);
bool carp_stack_peek(struct carp_stack *s, int64_t *v);
void carp_stack_print(struct carp_stack *s, FILE *fp);
void carp_stack_cleanup(struct carp_stack *s);

#endif
