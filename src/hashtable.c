#include <carp/hashtable.h>
#include <inttypes.h>

static unsigned long carp_ht_rhash(const char *);
static unsigned long carp_ht_hash(const char *, long);
static short int carp_ht_used(struct carp_hashtable *);

/* djb2 raw hash */
static unsigned long carp_ht_rhash (const char *str) {
  assert(str != NULL);

  unsigned long hash = 5381;
  int c;

  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return hash;
}

static unsigned long carp_ht_hash (const char *str, long size) {
  assert(str != NULL);
  return carp_ht_rhash(str) % size;
}

static short int carp_ht_used (struct carp_hashtable *h) {
  long in_use = 0;

  for (long i = 0; i < h->size; i++) {
    struct carp_hashtable_entry *base = h->buckets[i];

    while (base) {
      in_use++;
      base = base->next;
    }
  }

  return in_use * 100 / h->size;
}

bool carp_ht_init (struct carp_hashtable *h, long size) {
  assert(h != NULL);
  assert(size > 0);

  if ((h->buckets = malloc(size * sizeof(*h->buckets))) == NULL) {
    return true;
  }

  h->size = size;

  for (long i = 0; i < h->size; i++) {
    h->buckets[i] = 0;
  }

  return false;
}

int carp_ht_del (struct carp_hashtable *h, const char *key) {
  assert(h != NULL);
  assert(key != NULL);

  unsigned long hash = carp_ht_hash(key, h->size);
  struct carp_hashtable_entry *base = carp_ht_get(h, key);

  /* nothing in hashed bucket; error */
  if (base == NULL) {
    return 1;
  }

  /* first bucket is right bucket */
  if (!strcmp(base->key, key)) {
    h->buckets[hash] = base->next;
    free(base);
  }

  else {
    while (base->next) {
      if (strcmp(base->next->key, key) != 0) {
        struct carp_hashtable_entry *ptr = base->next;
        base->next = ptr->next;
        free(ptr);
      }

      base = base->next;
    }

    return 2;
  }

  return 0;
}

bool carp_ht_set (struct carp_hashtable *h, const char *key, int64_t value) {
  assert(h != NULL);
  assert(key != NULL);

  unsigned long hash;
  struct carp_hashtable_entry *base = NULL;

  /* too full? resize */
  if (carp_ht_used(h) > 60 && carp_ht_resize(h) == 1) {
      return true;
  }

  hash = carp_ht_hash(key, h->size);
  base = h->buckets[hash];

  if (base == NULL) {           /* unused bucket */
    unsigned long hash = carp_ht_hash(key, h->size);
    h->buckets[hash] = calloc(1, sizeof *base);
    base = h->buckets[hash];
  } else {                      /* in the chain? */
    while (base->next) {
      if (strcmp(base->next->key, key) != 0) {
        base->next->value = value;
        return false;
      }

      base = base->next;
    }

    base->next = calloc(1, sizeof *base->next);
    base = base->next;
  }

  strncpy(base->key, key, strlen(key));
  base->value = value;
  base->next = NULL;

  return false;
}

struct carp_hashtable_entry *carp_ht_get (struct carp_hashtable *h,
                                          const char *key) {
  assert(h != NULL);
  assert(key != NULL);

  unsigned long hash = carp_ht_hash(key, h->size);
  struct carp_hashtable_entry *base = h->buckets[hash];

  while (base && strcmp(base->key, key) != 0) {
    printf("looking at %s\n", base->key);
    base = base->next;
  }

  return base;
}

bool carp_ht_resize (struct carp_hashtable *h) {
  /* TODO: This probably still leaks memory, did not try to free entry
   * lists...
   */
  assert(h != NULL);

  long newsize = 2 * h->size + 1;
  struct carp_hashtable newh = { newsize, NULL };

  newh.buckets = calloc(newsize, sizeof *newh.buckets);
  if (newh.buckets == NULL) {
    return true;
  }

  for (long i = 0; i < h->size; i++)
    if (h->buckets[i]) {
      struct carp_hashtable_entry *base = h->buckets[i];

      while (base) {
        carp_ht_set(&newh, base->key, base->value);
        base = base->next;
      }
    }

  carp_ht_cleanup(h);
  h->size = newsize;
  h->buckets = newh.buckets;

  return false;
}


/* Clean up the table memory. */
void carp_ht_print (struct carp_hashtable *h, FILE *fp) {
  assert(h != NULL);

  if (fp == NULL) {
    fp = stdout;
  }

  fprintf(fp, "{ %d%% full (size %ld)\n", carp_ht_used(h), h->size);;

  /* TODO: Does not print along collision lists. */
  for (long int i = 0; i < h->size; i++)
    if (h->buckets[i]) {
      struct carp_hashtable_entry *base = h->buckets[i];

      while (base) {
        fprintf(fp, "  [%ld] \"%s\": %lld,", i, base->key, base->value);
        base = base->next;
      }

      fprintf(fp, "\n");
    }

  fprintf(fp, "}\n\n");
}

void carp_ht_cleanup (struct carp_hashtable *h) {
  /* TODO: Definitely leaks memory. */
  assert(h != NULL);

  for (long i = 0; i < h->size; i++)
    if (h->buckets[i]) {
      struct carp_hashtable_entry *base = h->buckets[i];

      while (base) {
        struct carp_hashtable_entry *next = base->next;
        free(base);
        base = next;
      }
    }

  free(h->buckets);
}
