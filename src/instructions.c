#include <carp/instructions.h>
#include <carp/machine.h>
#include <inttypes.h>

typedef int64_t CarpBinOpFunc(int64_t a, int64_t b);
typedef int64_t (*CarpBinOpFuncPtr)(int64_t a, int64_t b);

inline CarpBinOpFunc carp_instructions_add;
inline CarpBinOpFunc carp_instructions_sub;
inline CarpBinOpFunc carp_instructions_mul;
inline CarpBinOpFunc carp_instructions_mod;
inline CarpBinOpFunc carp_instructions_shr;
inline CarpBinOpFunc carp_instructions_shl;
inline CarpBinOpFunc carp_instructions_xor;
inline CarpBinOpFunc carp_instructions_or;
inline CarpBinOpFunc carp_instructions_and;
inline CarpBinOpFunc carp_instructions_lt;
inline CarpBinOpFunc carp_instructions_gt;

inline void carp_instructions_spop(struct carp_machine_state *m, int64_t *var);
inline void carp_instructions_spush(struct carp_machine_state *m, int64_t var);
inline void carp_instructions_binop(struct carp_machine_state *m,
                                    CarpBinOpFuncPtr op);

char carp_reverse_instr[][6] = {
  "halt",
  "nop",
  "loadr",
  "load",
  "store",
  "mov",
  "add",
  "sub",
  "mul",
  "mod",
  "shr",
  "shl",
  "not",
  "xor",
  "or",
  "and",
  "incr",
  "decr",
  "inc",
  "dec",
  "pushr",
  "push",
  "pop",
  "cmp",
  "lt",
  "gt",
  "jz",
  "rjz",
  "jnz",
  "rjnz",
  "jmp",
  "rjmp",
  "call",
  "ret",
  "preg",
  "ptop",
  "undef",
};

int64_t carp_instructions_add(int64_t a, int64_t b) {
  return a + b;
}

int64_t carp_instructions_sub(int64_t a, int64_t b) {
  return a - b;
}

int64_t carp_instructions_mul(int64_t a, int64_t b) {
  return a * b;
}

int64_t carp_instructions_mod(int64_t a, int64_t b) {
  return a % b;
}

int64_t carp_instructions_shr(int64_t a, int64_t b) {
  return a >> b;
}

int64_t carp_instructions_shl(int64_t a, int64_t b) {
  return a << b;
}

int64_t carp_instructions_xor(int64_t a, int64_t b) {
  return a ^ b;
}

int64_t carp_instructions_or(int64_t a, int64_t b) {
  return a | b;
}

int64_t carp_instructions_and(int64_t a, int64_t b) {
  return a & b;
}

int64_t carp_instructions_lt(int64_t a, int64_t b) {
  return a < b;
}

int64_t carp_instructions_gt(int64_t a, int64_t b) {
  return a > b;
}

void carp_instructions_spop(struct carp_machine_state *m, int64_t *var) {
  if (carp_stack_pop(&m->stack, var) == 1) {
    carp_vm_err(m, CARP_STACK_EMPTY);
  }
}

void carp_instructions_spush(struct carp_machine_state *m, int64_t var) {
  if (carp_stack_push(&m->stack, var) == 1) {
    carp_vm_err(m, CARP_STACK_NO_MEM);
  }
}

void carp_instructions_binop(struct carp_machine_state *m,
                             CarpBinOpFuncPtr op) {
  int64_t a, b;
  carp_instructions_spop(m, &b);
  carp_instructions_spop(m, &a);
  carp_instructions_spush(m, op(a, b));
}

void carp_instr_HALT(struct carp_machine_state *m) {
  carp_vm_exit(m, m->regs[CARP_EXT]);
}

void carp_instr_NOP(__attribute__((unused)) struct carp_machine_state *m) {
}

void carp_instr_LOADR(struct carp_machine_state *m) {
  int64_t reg = carp_vm_next(m),
    val = carp_vm_next(m);
  carp_reg_set(m->regs, reg, val);
}

void carp_instr_LOAD(struct carp_machine_state *m) {
  int64_t reladdr = carp_vm_next(m),
    fp = m->regs[CARP_FP],
    val = m->stack.contents[fp + reladdr];
  carp_instructions_spush(m, val);
}

void carp_instr_STORE(struct carp_machine_state *m) {
  int64_t reladdr = carp_vm_next(m),
    val = carp_vm_next(m),
    fp = m->regs[CARP_FP];
  m->stack.contents[fp + reladdr] = val;
}

void carp_instr_MOV(struct carp_machine_state *m) {
  int64_t dst = carp_vm_next(m),
    src = carp_vm_next(m);
  carp_reg_set(m->regs, dst, m->regs[src]);
}

void carp_instr_ADD(struct carp_machine_state *m) {
  carp_instructions_binop(m, carp_instructions_add);
}

void carp_instr_SUB(struct carp_machine_state *m) {
  carp_instructions_binop(m, carp_instructions_sub);
}

void carp_instr_MUL(struct carp_machine_state *m) {
  carp_instructions_binop(m, carp_instructions_mul);
}

void carp_instr_MOD(struct carp_machine_state *m) {
  carp_instructions_binop(m, carp_instructions_mod);
}

void carp_instr_SHR(struct carp_machine_state *m) {
  carp_instructions_binop(m, carp_instructions_shr);
}

void carp_instr_SHL(struct carp_machine_state *m) {
  carp_instructions_binop(m, carp_instructions_shl);
}

void carp_instr_NOT(struct carp_machine_state *m) {
  int64_t val;
  carp_instructions_spop(m, &val);
  carp_instructions_spush(m, ~val);
}

void carp_instr_XOR(struct carp_machine_state *m) {
  carp_instructions_binop(m, carp_instructions_xor);
}

void carp_instr_OR(struct carp_machine_state *m) {
  carp_instructions_binop(m, carp_instructions_or);
}

void carp_instr_AND(struct carp_machine_state *m) {
  carp_instructions_binop(m, carp_instructions_and);
}

void carp_instr_INCR(struct carp_machine_state *m) {
  carp_reg_inc(m->regs, carp_vm_next(m));
}

void carp_instr_DECR(struct carp_machine_state *m) {
  carp_reg_dec(m->regs, carp_vm_next(m));
}

void carp_instr_INC(struct carp_machine_state *m) {
  int64_t a;
  carp_instructions_spop(m, &a);
  carp_instructions_spush(m, ++a);
}

void carp_instr_DEC(struct carp_machine_state *m) {
  int64_t a;
  carp_instructions_spop(m, &a);
  carp_instructions_spush(m, --a);
}

void carp_instr_PUSHR(struct carp_machine_state *m) {
  carp_instructions_spush(m, *carp_reg_get(m->regs, carp_vm_next(m)));
}

void carp_instr_PUSH(struct carp_machine_state *m) {
  carp_instructions_spush(m, carp_vm_next(m));
}

void carp_instr_POP(struct carp_machine_state *m) {
  int64_t val;
  carp_instructions_spop(m, &val);
  carp_reg_set(m->regs, carp_vm_next(m), val);
}

void carp_instr_CMP(struct carp_machine_state *m) {
  int64_t b, a;
  carp_instructions_spop(m, &b);
  carp_instructions_spop(m, &a);
  carp_instructions_spush(m, a - b);
}

void carp_instr_LT(struct carp_machine_state *m) {
  carp_instructions_binop(m, carp_instructions_lt);
}

void carp_instr_GT(struct carp_machine_state *m) {
  carp_instructions_binop(m, carp_instructions_gt);
}

void carp_instr_JZ(struct carp_machine_state *m) {
  int64_t a;

  carp_instructions_spop(m, &a);

  if (!a) {                     /* zero */
    carp_reg_set(m->regs, CARP_IP, carp_vm_next(m));
  } else {                      /* next arg not an instr */
    carp_vm_next(m);
  }
}

void carp_instr_RJZ(struct carp_machine_state *m) {
  int64_t a;

  carp_instructions_spop(m, &a);

  if (!a) {                     /* zero */
    carp_reg_add(m->regs, CARP_IP, m->code[m->regs[CARP_IP] + 1]);
  } else {                      /* next arg not an instr */
    carp_vm_next(m);
  }
}

void carp_instr_JNZ(struct carp_machine_state *m) {
  int64_t a;

  carp_instructions_spop(m, &a);

  if (a) {                      /* not zero */
    carp_reg_set(m->regs, CARP_IP, carp_vm_next(m));
  } else {                      /* next arg not an instr */
    carp_vm_next(m);
  }
}

void carp_instr_RJNZ(struct carp_machine_state *m) {
  int64_t a;

  carp_instructions_spop(m, &a);

  if (a) {                      /* not zero */
    carp_reg_add(m->regs, CARP_IP, m->code[m->regs[CARP_IP] + 1]);
  } else {                      /* next arg not an instr */
    carp_vm_next(m);
  }
}

void carp_instr_JMP(struct carp_machine_state *m) {
  carp_reg_set(m->regs, CARP_IP, carp_vm_next(m));
}

void carp_instr_RJMP(struct carp_machine_state *m) {
  carp_reg_add(m->regs, CARP_IP, m->code[m->regs[CARP_IP] + 1]);
}

void carp_instr_CALL(struct carp_machine_state *m) {
  int64_t addr = carp_vm_next(m);

  carp_instructions_spush(m, m->regs[CARP_FP]);
  carp_instructions_spush(m, m->regs[CARP_IP]);

  carp_reg_set(m->regs, CARP_FP, m->regs[CARP_SP]);
  carp_reg_set(m->regs, CARP_IP, addr - 1);
}

void carp_instr_RET(struct carp_machine_state *m) {
  int64_t state;

  carp_reg_set(m->regs, CARP_SP, m->regs[CARP_FP]);

  carp_instructions_spop(m, &state);
  carp_reg_set(m->regs, CARP_IP, state);

  carp_instructions_spop(m, &state);
  carp_reg_set(m->regs, CARP_FP, state);
}

void carp_instr_PREG(struct carp_machine_state *m) {
  printf("%" PRId64 "\n", *carp_reg_get(m->regs, carp_vm_next(m)));
}

void carp_instr_PTOP(struct carp_machine_state *m) {
  int64_t val;

  if (carp_stack_peek(&m->stack, &val) == 1)
    carp_vm_err(m, CARP_STACK_EMPTY);

  printf("%" PRId64 "\n", val);
}
