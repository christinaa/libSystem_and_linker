/*
 * Core Framework: Linker: lnk_enter.s
 * Copyright (c) 2012 Christina Brooks
 *
 * Main entry point. After lnk is loaded, the kernel will
 * execute '__Start'. See CRT docs for the inital stack layout
 * details.
 */



#ifndef __arm__
	#error Only ARM architecture is supported.
#endif

.data
.align 2

/*  __DATA is not pie so we can use this to find out by 
 *  how much the kernel slid the linker by looking 
 *  at the PC
 */
__start_static_picbase: 
	.long	L__start_picbase

.text
.align 2

/*
 * early, stack based function that prints "ABRT\n"
 */
.globl __printAbrt
__printAbrt:
	sub	sp, #64

	mov r0, #65
	strb r0, [sp]
	mov r0, #66
	strb r0, [sp, #1]
	mov r0, #82
	strb r0, [sp, #2]
	mov r0, #84
	strb r0, [sp, #3]
	mov r0, #10
	strb r0, [sp, #4]

	mov r0, #0
	mov r1, sp
	mov r2, #5

	stmfd sp!, {r4,r7}
	mov r7, #4
	svc 0
	ldmfd sp!, {r4, r7}

	add sp, #64

	bx lr

.align 2
.globl __Start

/*
 * entry point
 * this is what the kernel tries to call after it loads the binary
 */
__Start:
	mov	r8, sp	/* Save SP on R8 */
	sub	sp, #24	/* SP-8 for the future lnk_header pointer */
	bic     sp, sp, #7

	ldr	r3, L__start_picbase_ptr
L__start_picbase:
	/*
	 * work out the slide
	 */
	sub	r0, pc, #8	/* PC */
	ldr	r3, [r0, r3] /* expected PC from a data bit */
	sub	r3, r0, r3	/* one minus the other = slide */

	/* Get arguments from the old stack */
	ldr	r0, [r8]	/* r0: mach_header */
	ldr	r1, [r8, #4]	/* r1: argc */
	ldr	r2, [r8, #8]	/* r2: argv */

	/* Fifth arg on SP (lnk_header) */
	ldr r4, [r8, #12] 
	str	r4, [sp, #0]

	/*
	 * Kick off the linker bootstrapping code.
	 * 
	 * With this compiler, the mangled name for 'lnk::bootstrap' is:
	 * __ZN3lnk9bootstrap5startEPK11mach_headeriPPKclS3_
	 */
	bl __ZN3lnk9bootstrap5startEPK11mach_headeriPPKclS3_

	/* create a new stack frame and fill it */
	sub	sp, r8, #12
	bx	r0

.align 2
L__start_picbase_ptr:
	.long	__start_static_picbase-L__start_picbase