#ifndef CARP_REGISTERS_H
#define CARP_REGISTERS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/* make register numbers easier on the eyes */
typedef enum {
  CARP_REG_UNDEF = -1,

  /* regs 0 through 9 for general use */
  CARP_R0,
  CARP_R1,
  CARP_R2,
  CARP_R3,
  CARP_R4,
  CARP_R5,
  CARP_R6,
  CARP_R7,
  CARP_R8,
  CARP_R9,

  /* regs used for cmp, et al... possibly */
  CARP_AX,
  CARP_BX,
  CARP_CX,
  CARP_DX,
  CARP_RX,                      /* remainder */

  CARP_IP,                      /* instruction pointer */
  CARP_SP,                      /* stack pointer */
  CARP_FP,                      /* frame pointer */
  CARP_GBG,                     /* garbage reg for pop */
  CARP_RUN,                     /* machine running status */
  CARP_EXT,                     /* exit code */

  CARP_NUM_REGS,
} carp_reg;

extern const char *carp_reverse_reg[];

int64_t *carp_reg_get(int64_t [], carp_reg);
bool carp_reg_set(int64_t [], carp_reg, int64_t);
bool carp_reg_add(int64_t [], carp_reg, int64_t);
bool carp_reg_sub(int64_t [], carp_reg, int64_t);
bool carp_reg_inc(int64_t [], carp_reg);
bool carp_reg_dec(int64_t [], carp_reg);
void carp_reg_print(int64_t [], FILE *);
void carp_reg_init(int64_t []);

#endif
