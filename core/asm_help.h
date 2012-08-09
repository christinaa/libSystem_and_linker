/*
 * Assembler helper functions.
 */

#ifndef	_ARCH_ARM_ASM_HELP_H_
#define	_ARCH_ARM_ASM_HELP_H_

#define	X_LEAF(name, value)				\
	.globl	name					;\
	.set	name,value

#define BRANCH_EXTERNAL(var)		;\
.globl	var							;\
	b	var

#define CALL_EXTERNAL(var)			\
	.globl	var						;\
	bl	var

#define ENTRY_POINT(name)			\
	.align 2						;\
	.globl  name					;\
	.text							;\
name:

#endif
