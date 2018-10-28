#include <carp/hashtable.h>
#include <test/test.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

int main () {
  bool status;
  bool test_res = true;
  struct carp_hashtable h;
  struct carp_hashtable_entry *res;
  const char *key = "mykey";
  const char *badkey = "badkey";
  int64_t value = 17;

  carp_ht_init(&h, 10);

  status = carp_ht_del(&h, key);
  test_res &= test(status == 1,
                   "Delete fails with empty table",
                   "Delete didn't fail with empty table, status = %d",
                   status);

  status = carp_ht_set(&h, key, value);
  test_res &= test(status == 0,
                   "Set returns successfully",
                   "Set returns unsuccessfully, status = %d",
                   status);

  res = carp_ht_get(&h, key);
  test_res &= test(res->value == value && !strcmp(key, res->key),
                   "Gets correct struct back",
                   "Gets incorrect struct back, "
                   "res->value = %d, value = %d, "
                   "key = %s, res->key = %s",
                   res->value, value,
                   key, res->key);

  res = carp_ht_get(&h, badkey);
  test_res &= test(res == NULL,
                   "Bad key gives null response",
                   "Bad key doesn't give null response, res = %p",
                   res);

  carp_ht_cleanup(&h);

  return !test_res;
}
