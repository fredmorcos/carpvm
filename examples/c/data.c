#include <carp/carp.h>

int main (__attribute__((unused)) int argc,
          __attribute__((unused)) char **argv) {
  int64_t strp;
  int64_t ecode;
  carp_machine_state m;

  char data[][CARP_HT_KEY_LENGTH] = {
    "max"
  };

  int64_t code[] = {
    /* CARP_INSTR_DBS, strp, 10, */
    /* CARP_INSTR_DBG, strp, CARP_R0, */
    CARP_INSTR_PREG, CARP_R0,
    CARP_INSTR_HALT, 0
  };

  strp = (int64_t) &data[0];

  carp_vm_init(&m, 1, 0);
  carp_vm_load(&m, code, sizeof(code)/sizeof(*code));
  ecode = carp_vm_run(&m);
  carp_vm_cleanup(&m);

  return ecode;
}
