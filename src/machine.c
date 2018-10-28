#include <carp/lexer.h>
#include <carp/machine.h>
#include <stdlib.h>
#include <assert.h>

static const CARPInstrFuncPtr carp_instructions[] = {
  [CARP_INSTR_HALT]  = carp_instr_HALT,
  [CARP_INSTR_NOP]   = carp_instr_NOP,
  [CARP_INSTR_LOADR] = carp_instr_LOADR,
  [CARP_INSTR_LOAD]  = carp_instr_LOAD,
  [CARP_INSTR_STORE] = carp_instr_STORE,
  [CARP_INSTR_MOV]   = carp_instr_MOV,
  [CARP_INSTR_ADD]   = carp_instr_ADD,
  [CARP_INSTR_SUB]   = carp_instr_SUB,
  [CARP_INSTR_MUL]   = carp_instr_MUL,
  [CARP_INSTR_MOD]   = carp_instr_MOD,
  [CARP_INSTR_SHR]   = carp_instr_SHR,
  [CARP_INSTR_SHL]   = carp_instr_SHL,
  [CARP_INSTR_NOT]   = carp_instr_NOT,
  [CARP_INSTR_XOR]   = carp_instr_XOR,
  [CARP_INSTR_OR]    = carp_instr_OR,
  [CARP_INSTR_AND]   = carp_instr_AND,
  [CARP_INSTR_INCR]  = carp_instr_INCR,
  [CARP_INSTR_DECR]  = carp_instr_DECR,
  [CARP_INSTR_INC]   = carp_instr_INC,
  [CARP_INSTR_DEC]   = carp_instr_DEC,
  [CARP_INSTR_PUSHR] = carp_instr_PUSHR,
  [CARP_INSTR_PUSH]  = carp_instr_PUSH,
  [CARP_INSTR_POP]   = carp_instr_POP,
  [CARP_INSTR_CMP]   = carp_instr_CMP,
  [CARP_INSTR_LT]    = carp_instr_LT,
  [CARP_INSTR_GT]    = carp_instr_GT,
  [CARP_INSTR_JZ]    = carp_instr_JZ,
  [CARP_INSTR_RJZ]   = carp_instr_RJZ,
  [CARP_INSTR_JNZ]   = carp_instr_JNZ,
  [CARP_INSTR_RJNZ]  = carp_instr_RJNZ,
  [CARP_INSTR_JMP]   = carp_instr_JMP,
  [CARP_INSTR_RJMP]  = carp_instr_RJMP,
  [CARP_INSTR_CALL]  = carp_instr_CALL,
  [CARP_INSTR_RET]   = carp_instr_RET,
  [CARP_INSTR_PREG]  = carp_instr_PREG,
  [CARP_INSTR_PTOP]  = carp_instr_PTOP
};

/* Initialize the VM - this includes lots of allocation */
void carp_vm_init (struct carp_machine_state *m,
                   size_t stack_height,
                   int64_t main_addr) {
  assert(m != NULL);
  assert(stack_height > 0);

  /* set all registers to 0, including:
   * - IP
   * - SP
   */
  carp_reg_init(m->regs);

  /* entrypoint  (main):  -1 because  IP  is  incremented before  each
   * instr.
   */
  carp_reg_set(m->regs, CARP_IP, main_addr - 1);

  /* turn VM on */
  m->regs[CARP_RUN] = 1;

  /* initialize stack and give pointer to SP */
  if (carp_stack_init(&m->stack, &m->regs[CARP_SP], stack_height) == 1) {
    carp_vm_err(m, CARP_STACK_NO_MEM);
  }

  /* initialize label hash table */
  carp_ht_init(&m->labels, 10);
}

/* VM initialization for parsed code (since the labels will have
 * already been generated).  Would like to rename this at some
 * point.
 */
void carp_vm_make (struct carp_machine_state *m) {
  assert(m != NULL);
  assert(&m->labels != NULL);

  carp_reg_init(m->regs);

  struct carp_hashtable_entry *res = carp_ht_get(&m->labels, "main");
  if (!res) {
    carp_vm_err(m, CARP_VM_NO_MAIN);
  } else {
    carp_reg_set(m->regs, CARP_IP, res->value - 1);
    m->regs[CARP_RUN] = 1;
  }


  if (carp_stack_init(&m->stack, &m->regs[CARP_SP], 1) == 1) {
    carp_vm_err(m, CARP_STACK_NO_MEM);
  }
}

/* Allocate space for the code, then load it */
void carp_vm_load (struct carp_machine_state *m,
                   int64_t code[],
                   int64_t length) {
  assert(m != NULL);

  if ((m->code = calloc(length, sizeof(*code))) == NULL) {
    carp_vm_err(m, CARP_CODE_NO_MEM);
  }

  memcpy(m->code, code, length * sizeof(*code));
}

/* Get the next instruction and execute it */
void carp_vm_eval (struct carp_machine_state *m) {
  assert(m != NULL);

  int64_t instr;

  carp_reg_inc(m->regs, CARP_IP);

  /* fetch instruction */
  instr = m->code[m->regs[CARP_IP]];

  /* decode, execute */
  carp_instructions[instr](m);
}

/* Start the fetch, decode, execute loop, then exit when done */
int64_t carp_vm_run(struct carp_machine_state *m) {
  assert(m != NULL);

  while (m->regs[CARP_RUN]) {
#ifdef CDEBUG
    int64_t instr = m->code[m->regs[CARP_IP] + 1];
    printf("i: %s\n", carp_reverse_instr[instr]);
#endif
    carp_vm_eval(m);
  }

  return m->regs[CARP_EXT];
}

/* Increment the instruction pointer and return the next value in the code */
int64_t carp_vm_next(struct carp_machine_state *m) {
  assert(m != NULL);
  return m->code[++m->regs[CARP_IP]];
}

/* Writes an error message to stderr, then signals for the machine to
 * cleanly exit.
 */
void carp_vm_err(struct carp_machine_state *m, const char *const e) {
  fprintf(stderr, "%s", e);
  putchar('\n');
  carp_vm_exit(m, EXIT_FAILURE);
}

/* Free the code, stack, varible table, and label table */
void carp_vm_cleanup(struct carp_machine_state *m) {
  assert(m != NULL);

  free(m->code);
  carp_stack_cleanup(&m->stack);
  carp_ht_cleanup(&m->labels);
}

/* Change the machine state to OFF, clean up, and exit */
void carp_vm_exit(struct carp_machine_state *m, int code) {
  assert(m != NULL);

  m->regs[CARP_RUN] = 0;
  m->regs[CARP_EXT] = code;
}

/* Read, tokenize, lex, and execute the contents of a Carp file */
int64_t carp_run_program(const char *fn) {
  struct carp_machine_state m;
  struct carp_tok *tokens = carp_lex_tokenize(fn);

  if (tokens == NULL) {
    fprintf(stderr, "Something went wrong with tokenization.\n");
    exit(EXIT_FAILURE);
  }

  carp_lex_lex(&m, tokens);
  return carp_vm_run(&m);
}
