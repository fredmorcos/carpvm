#include <carp/carp.h>

int main (__attribute__((unused)) int argc,
          __attribute__((unused)) char **argv) {
  carp_machine_state m;
  int64_t ecode;
  int64_t hellop;

  char data[][CARP_HT_KEY_LENGTH] = {
    "hello",
    "world"
  };

  // I wish there was a better way to do this.
  hellop = (int64_t) &data[0];

  int64_t code[] = {
    /* 3 + 9 + 7 == 19? */
    CARP_INSTR_LOADR,  CARP_R0, 7,
    CARP_INSTR_PREG,  CARP_R0,
    CARP_INSTR_PUSH,  3,
    CARP_INSTR_PUSH,  9,
    CARP_INSTR_ADD,
    CARP_INSTR_PTOP,

    CARP_INSTR_PUSHR, CARP_R0,
    CARP_INSTR_ADD,
    CARP_INSTR_PTOP,

    CARP_INSTR_PUSH,  19,
    CARP_INSTR_CMP,
    CARP_INSTR_PREG,  CARP_AX,

    /* set & get 20 */
    /* CARP_INSTR_DBS,   hellop, 20, */
    /* CARP_INSTR_DBG,   hellop, CARP_AX, */
    CARP_INSTR_PREG,  CARP_AX,

    /* should be in $? */
    CARP_INSTR_HALT, 1
  };

  carp_vm_init(&m, 10, 0);
  carp_vm_load(&m, code, sizeof(code)/sizeof(*code));
  ecode = carp_vm_run(&m);
  carp_vm_cleanup(&m);
  return ecode;
}
