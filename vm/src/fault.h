#ifndef SINTER_FAULT_H
#define SINTER_FAULT_H

#include <setjmp.h>

#include <sinter.h>

extern jmp_buf sinter_fault_jmp;

/**
 * Halts the VM with the given fault reason (using `longjmp`).
 *
 * Can only be called from a method within [sinter_run](@ref sinter_run).
 */
_Noreturn void sifault(enum sinter_fault);

#define SINTER_FAULTED() setjmp(sinter_fault_jmp)

#endif
