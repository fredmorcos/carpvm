#ifndef CARP_HASH_H
#define CARP_HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#define CARP_HT_KEY_LENGTH 15

struct carp_hashtable_entry {
  char key[CARP_HT_KEY_LENGTH];
  long long value;
  struct carp_hashtable_entry *next;
} carp_ht_entry;

struct carp_hashtable {
  long size;
  struct carp_hashtable_entry **buckets;
};

bool carp_ht_init(struct carp_hashtable *, long size);
int carp_ht_del(struct carp_hashtable *, const char *);
bool carp_ht_set(struct carp_hashtable *, const char *, int64_t);
struct carp_hashtable_entry *carp_ht_get(struct carp_hashtable *,
                                         const char *);
bool carp_ht_resize(struct carp_hashtable *);
void carp_ht_print(struct carp_hashtable *, FILE *);
void carp_ht_cleanup(struct carp_hashtable *);

#endif
