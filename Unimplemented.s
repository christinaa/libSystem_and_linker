/*
 * def leppard
 */

#define def_entry(x) .globl _##x; \
_##x

.text
.align 2

def_entry(pthread_atfork):
	bx lr

def_entry(LIBC_ABORT):
	b _abort

def_entry(im_really_bored):
	mov r0, #1	
	bx r0
