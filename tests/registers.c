#include <carp/registers.h>
#include <test/test.h>
#include <assert.h>

int main () {
  bool     status = 0;
  bool     test_res = true;
  int64_t  regs[CARP_NUM_REGS];
  int64_t  reg = 7;
  int64_t  value = 19;
  int64_t *res = NULL;
  int64_t  adder = 23;

  carp_reg_init(regs);
  for (int i = 0; i < CARP_NUM_REGS; i++) {
    status += regs[i];
  }
  test_res &= test(status == 0,
                   "All registers are set to 0",
                   "Some registers are not set to 0, status = %d",
                   status);

  status = carp_reg_set(regs, reg, value);
  test_res &= test(status == 0,
                   "Set reports no errors",
                   "Set reported errors, status = %d", status);
  test_res &= test(regs[reg] == value,
                   "Set is successful",
                   "Set is unsuccessful, regs[reg] = %d", value);

  res = carp_reg_get(regs, reg);
  test_res &= test(res != NULL,
                   "Get reports no errors",
                   "Get reports errors, res = NULL");
  test_res &= test(*res == value,
                   "Get is successful",
                   "Get is unsuccessful, *res = %d", value);

  status = carp_reg_add(regs, reg, adder);
  test_res &= test(status == 0,
                   "Add reports no errors",
                   "Add reports errors, status = %d", status);
  test_res &= test(regs[reg] == value + adder,
                   "Add is successful",
                   "Add is unsuccessful, regs[reg] = %d", value + adder);

  status = carp_reg_sub(regs, reg, adder);
  test_res &= test(status == 0,
                   "Sub reports no errors",
                   "Sub reports errors, status = %d", status);
  test_res &= test(regs[reg] == value,
                   "Sub is successful",
                   "Sub is unsuccessful, regs[reg] = %d", value);

  status = carp_reg_inc(regs, reg);
  test_res &= test(status == 0,
                   "Inc reports no errors",
                   "Inc reports errors, status = %d", status);
  test_res &= test(regs[reg] == value + 1,
                   "Inc is successful",
                   "Inc is unsuccessful, regs[reg] = %d", value + 1);

  status = carp_reg_dec(regs, reg);
  test_res &= test(status == 0,
                   "Dec reports no errors",
                   "Dec reports errors, status = %d", status);
  test_res &= test(regs[reg] == value,
                   "Dec is successful",
                   "Dec is unsuccessful, regs[reg] = %d", value);

  return !test_res;
}
