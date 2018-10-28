#include <carp/machine.h>
#include <test/test.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

int main () {
  struct carp_machine_state m;

  bool test_res = true;

  int64_t val0 = 127;
  int64_t val1 = 32;
  int64_t reg0 = CARP_R0;
  int64_t reg1 = CARP_R1;
  int64_t code[] = {
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_PUSH  , val1,
    CARP_INSTR_ADD   ,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_PUSH  , val1,
    CARP_INSTR_SUB   ,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_PUSH  , val1,
    CARP_INSTR_MUL   ,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_PUSH  , val1,
    CARP_INSTR_MOD   ,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_PUSH  , val1,
    CARP_INSTR_SHR   ,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_PUSH  , val1,
    CARP_INSTR_SHL   ,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_NOT   ,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_PUSH  , val1,
    CARP_INSTR_XOR   ,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_PUSH  , val1,
    CARP_INSTR_OR    ,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_PUSH  , val1,
    CARP_INSTR_AND   ,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_INC   ,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_DEC   ,
    CARP_INSTR_LOADR , reg0, val0,
    CARP_INSTR_PUSHR , reg0,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_POP   , reg1,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_PUSH  , val1,
    CARP_INSTR_CMP   ,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_PUSH  , val0,
    CARP_INSTR_CMP   ,
    CARP_INSTR_PUSH  , 1,
    CARP_INSTR_PUSH  , 2,
    CARP_INSTR_LT    ,
    CARP_INSTR_PUSH  , 2,
    CARP_INSTR_PUSH  , 1,
    CARP_INSTR_LT    ,
    CARP_INSTR_PUSH  , 2,
    CARP_INSTR_PUSH  , 1,
    CARP_INSTR_GT    ,
    CARP_INSTR_PUSH  , 1,
    CARP_INSTR_PUSH  , 2,
    CARP_INSTR_GT    ,
    CARP_INSTR_HALT  , EXIT_SUCCESS
  };

  carp_vm_init(&m, 1, 0);
  carp_vm_load(&m, code, sizeof(code)/sizeof(*code));
  carp_vm_run(&m);

  test_res &= test
    (m.stack.contents[0] == val0 + val1,
     "Stack ADD successful",
     "Stack ADD unsuccessful, contents[0] = %d, val0 = %d, val1 = %d",
     m.stack.contents[0], val0, val1);
  test_res &= test
    (m.stack.contents[1] == val0 - val1,
     "Stack SUB successful",
     "Stack SUB unsuccessful, contents[1] = %d, val0 = %d, val1 = %d",
     m.stack.contents[1], val0, val1);
  test_res &= test
    (m.stack.contents[2] == val0 * val1,
     "Stack MUL successful",
     "Stack MUL unsuccessful, contents[2] = %d, val0 = %d, val1 = %d",
     m.stack.contents[2], val0, val1);
  test_res &= test
    (m.stack.contents[3] == val0 % val1,
     "Stack MOD successful",
     "Stack MOD unsuccessful, contents[3] = %d, val0 = %d, val1 = %d",
     m.stack.contents[3], val0, val1);
  test_res &= test
    (m.stack.contents[4] == val0 >> val1,
     "Stack SHR successful",
     "Stack SHR unsuccessful, contents[4] = %d, val0 = %d, val1 = %d",
     m.stack.contents[4], val0, val1);
  test_res &= test
    (m.stack.contents[5] == val0 << val1,
     "Stack SHL successful",
     "Stack SHL unsuccessful, contents[5] = %d, val0 = %d, val1 = %d",
     m.stack.contents[5], val0, val1);
  test_res &= test
    (m.stack.contents[6] == ~val0,
     "Stack NOT successful",
     "Stack NOT unsuccessful, contents[6] = %d, val0 = %d",
     m.stack.contents[6], val0);
  test_res &= test
    (m.stack.contents[7] == (val0 ^ val1),
     "Stack XOR successful",
     "Stack XOR unsuccessful, contents[7] = %d, val0 = %d, val1 = %d",
     m.stack.contents[7], val0, val1);
  test_res &= test
    (m.stack.contents[8] == (val0 | val1),
     "Stack OR  successful",
     "Stack OR  unsuccessful, contents[8] = %d, val0 = %d, val1 = %d",
     m.stack.contents[8], val0, val1);
  test_res &= test
    (m.stack.contents[9] == (val0 & val1),
     "Stack AND successful",
     "Stack AND unsuccessful, contents[9] = %d, val0 = %d, val1 = %d",
     m.stack.contents[9], val0, val1);
  test_res &= test
    (m.stack.contents[10] == val0 + 1,
     "Stack INC successful",
     "Stack INC unsuccessful, contents[10] = %d, val0 = %d, val1 = %d",
     m.stack.contents[10], val0, val1);
  test_res &= test
    (m.stack.contents[11] == val0 - 1,
     "Stack DEC successful",
     "Stack DEC unsuccessful, contents[11] = %d, val0 = %d",
     m.stack.contents[11], val0);
  test_res &= test
    (m.stack.contents[12] == val0,
     "Stack PUSHR successful",
     "Stack PUSHR unsuccessful, contents[12] = %d, val0 = %d",
     m.stack.contents[12], val0);
  test_res &= test
    (m.stack.contents[13] == val0,
     "Stack PUSH successful",
     "Stack PUSH unsuccessful, contents[13] = %d, val0 = %d",
     m.stack.contents[13], val0);
  test_res &= test
    (m.regs[reg1] == val0,
     "Stack POP successful",
     "Stack POP unsuccessful, regs[reg1] = %d, val0 = %d",
     m.stack.contents[reg1], val0);
  test_res &= test
    (m.stack.contents[14] != 0,
     "Stack CMP != 0 successful",
     "Stack CMP != 0 unsuccessful, contents[14] = %d, val0 = %d",
     m.stack.contents[14], val0);
  test_res &= test
    (m.stack.contents[15] == 0,
     "Stack CMP == 0 successful",
     "Stack CMP == 0 unsuccessful, contents[15] = %d, val0 = %d",
     m.stack.contents[15], val0);
  test_res &= test
    (m.stack.contents[16] == 1,
     "Stack LT true successful",
     "Stack LT true unsuccessful, contents[16] = %d, val0 = %d",
     m.stack.contents[16], val0);
  test_res &= test
    (m.stack.contents[17] == 0,
     "Stack LT false successful",
     "Stack LT false unsuccessful, contents[17] = %d, val0 = %d",
     m.stack.contents[17], val0);
  test_res &= test
    (m.stack.contents[18] == 1,
     "Stack GT true successful",
     "Stack GT true unsuccessful, contents[18] = %d, val0 = %d",
     m.stack.contents[18], val0);
  test_res &= test
    (m.stack.contents[19] == 0,
     "Stack GT false successful",
     "Stack GT false unsuccessful, contents[19] = %d, val0 = %d",
     m.stack.contents[19], val0);

  carp_vm_cleanup(&m);

  return !test_res;
}
