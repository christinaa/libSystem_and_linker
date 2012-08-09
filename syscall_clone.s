/*
 * Core Framework: LibSyscall: syscall_clone.s
 * Copyright (c) 2012 Christina Brooks
 *
 * Support for the clone system call.
 */

#include "NRSyscall.h"

.text
.align 2

/*
 Clone.
 There is a bit of magic involved here.
 
 pid_t (int flags,
 void *child_stack,
 pid_t *pid,
 void *tls,
 pid_t *ctid,
 int (*fn)(void *),
 void* arg);
 */
.globl _clone$SYS$LINUX
_clone$SYS$LINUX:
	mov r12, sp

	stmfd sp!, {r4-r7}

	/**/
	ldr r5, [r12, #4] /* int (*fn)(void *) */
	ldr r6, [r12, #8] /* void* arg */

	str r5, [r1, #-4]
	str r6, [r1, #-8]


	mov r7, #120
	svc 0
	movs    r0, r0
	beq clone_child

	/* the parent just needs to return */
clone_parent:
	ldmfd sp!, {r4-r7}
	bx lr

	/* the child needs to jump to the new func  */
clone_child:
	ldr r0, [sp, #-4]
	ldr r1, [sp, #-8]

	sub sp, #8
	bx r0
