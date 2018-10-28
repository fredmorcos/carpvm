#include <carp/machine.h>
#include <test/test.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

int main () {
  int64_t code0[] = { CARP_INSTR_HALT, EXIT_SUCCESS };
  /* int64_t code1[] = { CARP_INSTR_HALT, EXIT_FAILURE }; */

  struct carp_machine_state m;
  /* struct carp_machine_state m1; */

  int64_t status;
  /* int64_t status1; */

  bool test_res = true;

  carp_vm_init(&m, 1, 0);
  carp_vm_load(&m, code0, sizeof(code0) / sizeof(*code0));
  status = carp_vm_run(&m);
  carp_vm_cleanup(&m);

  /* carp_vm_init(&m1, 1, 0); */
  /* carp_vm_load(&m1, code1, sizeof(code1) / sizeof(*code1)); */
  /* status1 = carp_vm_run(&m1); */
  /* carp_vm_cleanup(&m1); */

  test_res &= test
    (status == EXIT_SUCCESS,
     "Machine exits with no failures",
     "Machine exited with failures, status = %d",
     status);
  /* test_res &= test */
  /*   (status1 == EXIT_FAILURE, */
  /*    "Machine exits with failure", */
  /*    "Machine exited w/o failure (while not supposed to), status1 = %d", */
  /*    status1); */

  return !test_res;
}
