/*
 * Core Framework: LowLevelLock.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Low level locking routines for the Linux kernel.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "_lnk_log_c.h"
#include <errno.h>
#include <libkern/OSAtomic.h>
#include "OSUAtomic.h"
#include "OSThread.h"
#include "OSLog.h"

/* futex syscall (http://linux.die.net/man/2/futex) */
extern int
futex$LINUX(OSLowLock *uaddr, int op, int val, const struct timespec *timeout, int *uaddr2, int val3);

#define FUTEX_WAIT              0
#define FUTEX_WAKE              1
#define FUTEX_FD                2
#define FUTEX_REQUEUE           3
#define FUTEX_CMP_REQUEUE       4

#define FUTEX_PRIVATE_FLAG	128

#define lll_private_flag(fl, private) ((fl) | FUTEX_PRIVATE_FLAG)

/*
	foo
	(mem, newval, oldval)
 */

int lll_futex_timed_wait(OSLowLock* futex,
						 int value,
						 const struct timespec *timeout)
{
	/*
		If this is ever called in single threaded mode, the
		application will hang up (as this will become a deadlock).
	 */
	
	//OSLog("lll_futex_timed_wait: %p", futex);
	
	return futex$LINUX(futex,
					   FUTEX_WAIT,
					   value,
					   timeout,
					   NULL,
					   0);
}

int lll_futex_wait(OSLowLock* futex,
				   int value)
{
	return lll_futex_timed_wait(futex,
								value,
								NULL);
}

int lll_futex_wake(OSLowLock* futex,
				   int value)
{
	return futex$LINUX(futex,
					   FUTEX_WAKE,
					   value,
					   NULL,
					   NULL,
					   0);
}

void lll_lock_wait(OSLowLock* futex) {
	do {
		int old = OSCompareAndExchangeInt(futex, 2, 1);
		
		if (old != 0) {
			lll_futex_wait(futex, 2);
		}
	}
	while (OSCompareAndExchangeInt(futex, 2, 0) != 0);
}

int lll_trylock(OSLowLock* futex) {
	return OSCompareAndExchangeInt(futex, 1, 0);
}

void lll_unlock(OSLowLock* futex) {
	//OSLog("lll_unlock: %p", futex);
	int old = OSAtomicExchange(futex, 0);
	if (__builtin_expect(old > 1, 0)) {
		lll_futex_wake(futex, 1);
	}
}

void lll_lock(OSLowLock* futex) {
	//OSLog("lll_lock: %p", futex);
	if (__builtin_expect(OSCompareAndExchangeInt(futex, 1, 0), 0)) {
		lll_lock_wait(futex);
	}
}

// newline! \n

