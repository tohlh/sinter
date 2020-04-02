#include <assert.h>

#include <sinter/fault.h>
#include <sinter/vm.h>

jmp_buf sinter_fault_jmp = { 0 };

/**
 * Faults with the given reason. This will immediately abort execution.
 */
_Noreturn void sifault(sinter_fault_t reason) {
  sistate.fault_reason = reason;
#ifdef SINTER_ABORT_ON_FAULT
  assert("Faulting." == 0);
#endif
  longjmp(sinter_fault_jmp, 1);
}
