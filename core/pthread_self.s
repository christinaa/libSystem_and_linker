/*
 * armv7
 */

#include <arm/arch.h>

        .text
        .align 2
        .globl _pthread_self
_pthread_self:
	mrc	p15, 0, r0, c13, c0, 3
	bx	lr
