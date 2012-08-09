/*
 * Core Framework: Uatomic.c
 * Copyright (c) 2012 Christina Brooks
 *
 * XXX: This is GNU. This needs to be fixed.
 */

#include <sys/types.h>

void __arm_link_error (void);

/* Use the atomic builtins provided by GCC in case the backend provides
   a pattern to do this efficiently.  */

#ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
#define atomic_full_barrier() __sync_synchronize ()
#elif defined __thumb2__
#define atomic_full_barrier() \
     __asm__ __volatile__						      \
	     ("movw\tip, #0x0fa0\n\t"					      \
	      "movt\tip, #0xffff\n\t"					      \
	      "blx\tip"							      \
	      : : : "ip", "lr", "cc", "memory");
#else
#define atomic_full_barrier() \
     __asm__ __volatile__						      \
	     ("mov\tip, #0xffff0fff\n\t"				      \
	      "mov\tlr, pc\n\t"						      \
	      "add\tpc, ip, #(0xffff0fa0 - 0xffff0fff)"			      \
	      : : : "ip", "lr", "cc", "memory");
#endif

/* Atomic compare and exchange.  This sequence relies on the kernel to
   provide a compare and exchange operation which is atomic on the
   current architecture, either via cleverness on pre-ARMv6 or via
   ldrex / strex on ARMv6.  */

#define __arch_compare_and_exchange_val_8_acq(mem, newval, oldval) \
  ({ __arm_link_error (); oldval; })

#define __arch_compare_and_exchange_val_16_acq(mem, newval, oldval) \
  ({ __arm_link_error (); oldval; })

#ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
#define __arch_compare_and_exchange_val_32_acq(mem, newval, oldval) \
  __sync_val_compare_and_swap ((mem), (oldval), (newval))

/* It doesn't matter what register is used for a_oldval2, but we must
   specify one to work around GCC PR rtl-optimization/21223.  Otherwise
   it may cause a_oldval or a_tmp to be moved to a different register.  */

#elif defined __thumb2__
/* Thumb-2 has ldrex/strex.  However it does not have barrier instructions,
   so we still need to use the kernel helper.  */
#define OSCompareAndExchangeInt(mem, newval, oldval) \
  ({ register __typeof (oldval) a_oldval __asm__ ("r0");		      \
     register __typeof (oldval) a_newval __asm__ ("r1") = (newval);	      \
     register __typeof (mem) a_ptr __asm__ ("r2") = (mem);		      \
     register __typeof (oldval) a_tmp __asm__ ("r3");			      \
     register __typeof (oldval) a_oldval2 __asm__ ("r4") = (oldval);	      \
     __asm__ __volatile__						      \
	     ("0:\tldr\t%[tmp],[%[ptr]]\n\t"				      \
	      "cmp\t%[tmp], %[old2]\n\t"				      \
	      "bne\t1f\n\t"						      \
	      "mov\t%[old], %[old2]\n\t"				      \
	      "movw\t%[tmp], #0x0fc0\n\t"				      \
	      "movt\t%[tmp], #0xffff\n\t"				      \
	      "blx\t%[tmp]\n\t"						      \
	      "bcc\t0b\n\t"						      \
	      "mov\t%[tmp], %[old2]\n\t"				      \
	      "1:"							      \
	      : [old] "=&r" (a_oldval), [tmp] "=&r" (a_tmp)		      \
	      : [new] "r" (a_newval), [ptr] "r" (a_ptr),		      \
		[old2] "r" (a_oldval2)					      \
	      : "ip", "lr", "cc", "memory");				      \
     a_tmp; })
#else
#define OSCompareAndExchangeInt(mem, newval, oldval) \
  ({ register __typeof (oldval) a_oldval __asm__ ("r0");		      \
     register __typeof (oldval) a_newval __asm__ ("r1") = (newval);	      \
     register __typeof (mem) a_ptr __asm__ ("r2") = (mem);		      \
     register __typeof (oldval) a_tmp __asm__ ("r3");			      \
     register __typeof (oldval) a_oldval2 __asm__ ("r4") = (oldval);	      \
     __asm__ __volatile__						      \
	     ("0:\tldr\t%[tmp],[%[ptr]]\n\t"				      \
	      "cmp\t%[tmp], %[old2]\n\t"				      \
	      "bne\t1f\n\t"						      \
	      "mov\t%[old], %[old2]\n\t"				      \
	      "mov\t%[tmp], #0xffff0fff\n\t"				      \
	      "mov\tlr, pc\n\t"						      \
	      "add\tpc, %[tmp], #(0xffff0fc0 - 0xffff0fff)\n\t"		      \
	      "bcc\t0b\n\t"						      \
	      "mov\t%[tmp], %[old2]\n\t"				      \
	      "1:"							      \
	      : [old] "=&r" (a_oldval), [tmp] "=&r" (a_tmp)		      \
	      : [new] "r" (a_newval), [ptr] "r" (a_ptr),		      \
		[old2] "r" (a_oldval2)					      \
	      : "ip", "lr", "cc", "memory");				      \
     a_tmp; })
#endif

#define __arch_compare_and_exchange_val_64_acq(mem, newval, oldval) \
  ({ __arm_link_error (); oldval; })

#ifndef atomic_exchange_acq
# define OSAtomicExchange(mem, newvalue) \
({ __typeof (*(mem)) __atg5_oldval;					      \
__typeof (mem) __atg5_memp = (mem);				      \
__typeof (*(mem)) __atg5_value = (newvalue);			      \
\
do									      \
__atg5_oldval = *__atg5_memp;					      \
while (__builtin_expect						      \
(OSCompareAndExchangeInt(__atg5_memp, __atg5_value, \
__atg5_oldval), 0));	      \
\
__atg5_oldval; })
#endif

