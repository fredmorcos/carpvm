#include <carp/registers.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

const char *carp_reverse_reg[] = {
  "r0",
  "r1",
  "r2",
  "r3",
  "r4",
  "r5",
  "r6",
  "r7",
  "r8",
  "r9",
  "ax",
  "bx",
  "cx",
  "dx",
  "rx",
  "ip",
  "sp",
  "fp",
  "gbg",
  "run",
  "ext",
  "undef",
};

/* Returns a pointer to a register. Fails if register is outside bounds. */
int64_t *carp_reg_get(int64_t regs[], carp_reg reg) {
  assert(regs != NULL);

  if (reg >= CARP_NUM_REGS) {
    return NULL;
  }

  return &regs[reg];
}

/* Set the value in a given register. Fails if register is outside bounds. */
bool carp_reg_set(int64_t regs[], carp_reg reg, int64_t value) {
  assert(regs != NULL);

  int64_t *res = carp_reg_get(regs, reg);

  if (res == NULL) {
    return true;
  }

  *res = value;
  return false;
}

/* Adds a value to a register. */
bool carp_reg_add(int64_t regs[], carp_reg reg, int64_t value) {
  assert(regs != NULL);

  int64_t *res = carp_reg_get(regs, reg);

  if (res == NULL) {
    return true;
  }

  *res += value;
  return false;
}

/* Subtracts a value from a register. */
bool carp_reg_sub(int64_t regs[], carp_reg reg, int64_t value) {
  assert(regs != NULL);

  int64_t *res = carp_reg_get(regs, reg);

  if (res == NULL) {
    return true;
  }

  *res -= value;
  return false;
}

/* Increments a register. */
bool carp_reg_inc(int64_t regs[], carp_reg reg) {
  assert(regs != NULL);

  int64_t *res = carp_reg_get(regs, reg);

  if (res == NULL) {
    return true;
  }

  carp_reg_add(regs, reg, 1);
  return false;
}

/* Decrements a register. */
bool carp_reg_dec(int64_t regs[], carp_reg reg) {
  assert(regs != NULL);

  int64_t *res = carp_reg_get(regs, reg);

  if (res == NULL) {
    return true;
  }

  carp_reg_sub(regs, reg, 1);
  return false;
}

void carp_reg_print(int64_t regs[], FILE *fp) {
  assert(regs != NULL);

  if (fp == NULL) {
    fp = stdout;
  }

  fprintf(fp, "{\n");

  for (int i = 0; i < CARP_NUM_REGS; i++) {
    fprintf(fp, "%s: %" PRId64 ",\n", carp_reverse_reg[i], regs[i]);
  }

  fprintf(fp, "}\n");
}

/* Initialize all the registers to value 0. */
void carp_reg_init(int64_t regs[]) {
  assert(regs != NULL);
  memset(regs, 0, CARP_NUM_REGS * sizeof(*regs));
}
