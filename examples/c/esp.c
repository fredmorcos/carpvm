#include <carp/carp.h>

int main (__attribute__((unused)) int argc,
          __attribute__((unused)) char **argv) {
  carp_machine_state m;
  int64_t ecode;

  int64_t code[] = {
    CARP_INSTR_PUSH, 5,
    CARP_INSTR_PUSH, 6,
    CARP_INSTR_PREG,  CARP_SP,

    CARP_INSTR_DECR, CARP_SP,
    CARP_INSTR_POP, CARP_GBG,
    CARP_INSTR_PREG, CARP_GBG,

    CARP_INSTR_HALT, 1
  };

  carp_vm_init(&m, 10, 0);
  carp_vm_load(&m, code, sizeof(code)/sizeof(*code));
  ecode = carp_vm_run(&m);
  carp_vm_cleanup(&m);

  return ecode;
}
