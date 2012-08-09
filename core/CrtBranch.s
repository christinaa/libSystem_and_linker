/*
 * Core Framework: CrtBranch.s
 * Copyright (c) 2012 Christina Brooks
 *
 * C runtime library trampoline.
 * Refer back to CRT docs.
 */

/*
 * C runtime startup for ppc, ppc64, i386, x86_64
 *
 * Kernel sets up stack frame to look like:
 *
 *	       :
 *	| STRING AREA |
 *	+-------------+
 *	|      0      |	
 *	+-------------+	
 *	|  exec_path  | extra "apple" parameters start after NULL terminating env array
 *	+-------------+
 *	|      0      |
 *	+-------------+
 *	|    env[n]   |
 *	+-------------+
 *	       :
 *	       :
 *	+-------------+
 *	|    env[0]   |
 *	+-------------+
 *	|      0      |
 *	+-------------+
 *	| arg[argc-1] |
 *	+-------------+
 *	       :
 *	       :
 *	+-------------+
 *	|    arg[0]   |
 *	+-------------+
 *	|     argc    | argc is always 4 bytes long, even in 64-bit architectures
 *	+-------------+ <- sp
 *
 *	Where arg[i] and env[i] point into the STRING AREA
 */

.align 2

/*
	 Main trampoline.
	 void _call_main(int argc, char **argv, void* entry);
	 
	 Sets up argc and argv on the stack and jumps to the
	 address in r2 which should be the CRT entry point.
 */
.globl __call_main
__call_main:
	mov r5, #0

	/* APPLE */
	sub sp, #4
	str r5, [sp, #0]
	sub sp, #4
	str r5, [sp, #0]

	/* Environment */
	sub sp, #4
	str r5, [sp, #0]
	sub sp, #4
	str r5, [sp, #0]

	/* calculate size of the argv array (each is 4) */
	mov r4, #4
	mul r3, r0, r4

	/* get stack space for the argv array (and a NULL at the end) */
	sub sp, r3
	sub sp, #4
	mov r3, #0

	/* fill out the array on the stack */
Lfill_iter:
	ldr r4, [r1, r3]
	str r4, [sp, r3]
	cmp r4, #0
	add r3, #4
	bne Lfill_iter

	/* argc */
	sub sp, #4
	str r0, [sp, #0]

	blx r2

	/* oops, the crt entry function returned, we're fucked */
Lepic_fail:
	blx __printAbrt
	b Lepic_fail

/*
	CRT code:
 
	 #ifdef __arm__
 
start:
	 ldr	r0, [sp]		// get argc into r0
	 add	r1, sp, #4		// get argv into r1
	 add	r4, r0, #1		// calculate argc + 1 into r4
	 add	r2, r1, r4, lsl #2	// get address of env[0] into r2
	 bic	sp, sp, #7		// force eight-byte alignment
	 mov	r3, r2
 
Lapple:
	 ldr	r4, [r3], #4		// look for NULL ending env[] array
	 cmp	r4, #0
	 bne	Lapple			
	 // "apple" param now in r3
 */

/*
	int main(int argc, char *argv[])
*/

.align 2

/* XXX: TEMP */
.globl __DeadBabe
__DeadBabe:
	mov r0, pc
	mov r1, lr
	bl __HandleUndefinedSymbol

.globl dyld_stub_binder
dyld_stub_binder:
	bl __printAbrt
	b dyld_stub_binder

