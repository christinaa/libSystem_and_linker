/*
 * Core Framework: LibSyscall: syscall_compat.s
 * Copyright (c) 2012 Christina Brooks
 *
 * System calls that do not really need shims of any sort.
 */

#include "NRSyscall.h"

.text
.align 2

/*
	Darwin-compatible, directly mapped.
 */

#define declare_syscall(x, y) \
.globl _##x; \
_##x: \
stmfd   sp!, {r4, r7}; \
ldr     r7, =y;\
swi     #0;\
ldmfd   sp!, {r4, r7};\
bx    lr;

#define declare_syscall_low(x) declare_syscall(x, __NR_##x);

/* same nr entries */
declare_syscall_low(pipe);
declare_syscall_low(gettid);
declare_syscall_low(link);
declare_syscall_low(setreuid);
declare_syscall_low(setregid);
declare_syscall_low(setgid);
declare_syscall_low(sched_yield);
declare_syscall_low(umask);
declare_syscall_low(fchdir);
declare_syscall_low(alarm);
declare_syscall_low(setsid);
declare_syscall_low(setpgid);
declare_syscall_low(getppid);
declare_syscall_low(getpgrp)
declare_syscall_low(access);
declare_syscall_low(dup);
declare_syscall_low(chdir);
declare_syscall_low(kill);
declare_syscall_low(rename);
declare_syscall_low(geteuid);
declare_syscall_low(getpid);
declare_syscall_low(chmod);
declare_syscall_low(unlink);
declare_syscall_low(rmdir);
declare_syscall_low(readlink);
declare_syscall_low(mkdir);
declare_syscall_low(dup2);
declare_syscall_low(chroot);
declare_syscall_low(munmap);
declare_syscall_low(write);
declare_syscall_low(read);
declare_syscall_low(close);
declare_syscall_low(nanosleep);

declare_syscall_low(utime);
declare_syscall_low(fsync);
declare_syscall_low(symlink);
declare_syscall_low(ftruncate);
declare_syscall_low(getpriority);
declare_syscall_low(setpriority);

/* nr entries are different */
declare_syscall(getuid, __NR_getuid32);
declare_syscall(lseek_trap, __NR_lseek);
declare_syscall(chown, __NR_chown32);
declare_syscall(getgid, __NR_getgid32);

/* special */
SyscallEntry(_select)
	mov     ip, sp
	stmfd   sp!, {r4, r5, r6, r7}
	ldmfd   ip, {r4, r5, r6}
	ldr     r7, =__NR__newselect
	swi     #0
	ldmfd   sp!, {r4, r5, r6, r7}
	bx    lr

