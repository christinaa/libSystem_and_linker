#include <arm/arch.h>

#define	__APPLE_API_PRIVATE
//#include <machine/cpu_capabilities.h>
#undef	__APPLE_API_PRIVATE

// This routine is never called directly by user code, jumped from kernel
// args 0 to 3 are already in the regs 0 to 3
// should set stack with the 2 extra args before calling pthread_wqthread()
// arg4 is in r[4]
// arg5 is in r[5]

.text
.align 2
.globl _thread_start
_thread_start:
	stmfd sp!, {r4, r5}
	bl __pthread_start