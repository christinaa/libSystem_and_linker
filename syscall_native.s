/*
 * Core Framework: LibSyscall: syscall_native.s
 * Copyright (c) 2012 Christina Brooks
 *
 * Native syscalls used by the new kernel.
 */

#include "NRSyscall.h"

.align 2
.text

NativeSyscallEntry(_sigaction)
	stmfd   sp!, {r4, r7}
	ldr     r7, =__NR_sigaction
	swi     #0
	ldmfd   sp!, {r4, r7}
	bx    lr

NativeSyscallEntry(_sigprocmask)
	stmfd   sp!, {r4, r7}
	ldr     r7, =__NR_sigprocmask
	swi     #0
	ldmfd   sp!, {r4, r7}
	bx    lr