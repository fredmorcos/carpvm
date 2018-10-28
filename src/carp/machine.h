#ifndef CARP_MACHINE_H
#define CARP_MACHINE_H

#include <carp/stack.h>
#include <carp/hashtable.h>
#include <carp/registers.h>
#include <carp/instructions.h>

#define CARP_VM_NO_MAIN "Could not find main."
#define CARP_EXIT_STACK_FAILED "Could not initialize stack."
#define CARP_STACK_NO_MEM  "Not enough memory."
#define CARP_STACK_EMPTY "Carp stack is empty."
#define CARP_HT_DNE "Variable not defined."
#define CARP_HT_NO_MEM "Not enough memory."
#define CARP_HT_CONTENTS_NULL "Could not get. Contents NULL."
#define CARP_CODE_NO_MEM "Could not allocate memory for code."

struct carp_machine_state {
  struct carp_stack      stack;
  int64_t                regs[CARP_NUM_REGS];
  struct carp_hashtable  vars;
  struct carp_hashtable  labels;
  int64_t               *code;
};

void carp_vm_init(struct carp_machine_state *, size_t, int64_t);
void carp_vm_make(struct carp_machine_state *);
void carp_vm_load(struct carp_machine_state *, int64_t [], int64_t);
void carp_vm_eval(struct carp_machine_state *);
int64_t carp_vm_run(struct carp_machine_state *);
int64_t carp_vm_next(struct carp_machine_state *);
void carp_vm_err(struct carp_machine_state *, const char *const);
void carp_vm_cleanup(struct carp_machine_state *);
void carp_vm_exit(struct carp_machine_state *, int);

int64_t carp_run_program (const char *);

typedef void (CARPInstrFunc) (struct carp_machine_state *m);
typedef void (*CARPInstrFuncPtr) (struct carp_machine_state *m);

CARPInstrFunc
  carp_instr_HALT,
  carp_instr_NOP,
  carp_instr_LOADR,
  carp_instr_LOAD,
  carp_instr_STORE,
  carp_instr_MOV,
  carp_instr_ADD,
  carp_instr_SUB,
  carp_instr_MUL,
  carp_instr_MOD,
  carp_instr_SHR,
  carp_instr_SHL,
  carp_instr_NOT,
  carp_instr_XOR,
  carp_instr_OR,
  carp_instr_AND,
  carp_instr_INCR,
  carp_instr_DECR,
  carp_instr_INC,
  carp_instr_DEC,
  carp_instr_PUSHR,
  carp_instr_PUSH,
  carp_instr_POP,
  carp_instr_CMP,
  carp_instr_LT,
  carp_instr_GT,
  carp_instr_JZ,
  carp_instr_RJZ,
  carp_instr_JNZ,
  carp_instr_RJNZ,
  carp_instr_JMP,
  carp_instr_RJMP,
  carp_instr_DBS,
  carp_instr_DBG,
  carp_instr_CALL,
  carp_instr_RET,
  carp_instr_PREG,
  carp_instr_PTOP,
  carp_instr_PVARS;

#endif
