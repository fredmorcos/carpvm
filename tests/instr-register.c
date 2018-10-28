#include <carp/machine.h>
#include <test/test.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

int main () {
  struct carp_machine_state m;

  bool test_res = true;

  int64_t val = 127;
  int64_t code[] = {
    CARP_INSTR_LOADR , CARP_R0, val,
    CARP_INSTR_MOV   , CARP_R1, CARP_R0,
    CARP_INSTR_MOV   , CARP_R2, CARP_R0,
    CARP_INSTR_INCR  , CARP_R2,
    CARP_INSTR_MOV   , CARP_R3, CARP_R0,
    CARP_INSTR_DECR  , CARP_R3,
    CARP_INSTR_HALT  , EXIT_SUCCESS
  };

  carp_vm_init(&m, 1, 0);
  carp_vm_load(&m, code, sizeof(code)/sizeof(*code));
  carp_vm_run(&m);
  carp_vm_cleanup(&m);

  test_res &= test(m.regs[CARP_R0] == val,
                   "Set register successfully",
                   "Set register unsuccessfully, regs[CARP_R0] = %d",
                   val);
  test_res &= test(m.regs[CARP_R1] == val,
                   "Moved register successfully",
                   "Moved register unsuccessfully, regs[CARP_R1] = %d",
                   val);
  test_res &= test(m.regs[CARP_R2] == val + 1,
                   "Incremented register successfully",
                   "Incremented register unsuccessfully, regs[CARP_R2] = %d",
                   val + 1);
  test_res &= test(m.regs[CARP_R3] == val - 1,
                   "Decremented register successfully",
                   "Decremented register unsuccessfully, regs[CARP_R3] = %d",
                   val - 1);

  return !test_res;
}
