#include <carp/carp.h>

int main (__attribute__((unused)) int argc,
          __attribute__((unused)) char **argv) {
  carp_machine_state m;
  int64_t ecode;

  int64_t code[] = {
    CARP_INSTR_LOAD, -4,        /* 1st arg */
    CARP_INSTR_LOAD, -3,        /* 2nd arg */
    CARP_INSTR_ADD,
    CARP_INSTR_POP, CARP_AX,
    CARP_INSTR_RET,

    CARP_INSTR_PUSH, 7,         /* 1st arg */
    CARP_INSTR_PUSH, 9,         /* 2nd arg */
    CARP_INSTR_CALL, 0,

    CARP_INSTR_PREG, CARP_AX,

    CARP_INSTR_HALT, 0
  };


  carp_vm_init(&m, 1, 6);
  carp_vm_load(&m, code, sizeof(code) / sizeof(*code));
  ecode = carp_vm_run(&m);
  carp_vm_cleanup(&m);

  return ecode;
}
