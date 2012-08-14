/*
 * OSSpinLock
 * Copyright (c) 2012 Christina Brooks
 *
 * OSSpinLock interface to the LLLs.
 */

#include <stdio.h>
#include <libkern/OSAtomic.h>
#include "OSLog.h"
#include "OSThread.h"

/*
	Basically, this should work.
 
	I don't know if this is how it should be implemented
	but I think this is it.
 */

void __reserved_OSAtomic_1(void) {
	
}


_Bool OSSpinLockTry(volatile OSSpinLock *__lock)
{
	return lll_trylock((OSLowLock*)__lock);
}

int _spin_lock_try(volatile OSSpinLock *__lock)
{
	return lll_trylock((OSLowLock*)__lock);
}

void _spin_lock(volatile OSSpinLock *__lock)
{
	lll_lock((OSLowLock*)__lock);
}

void _spin_unlock(volatile OSSpinLock *__lock)
{
	lll_unlock((OSLowLock*)__lock);
}

void OSSpinLockLock(volatile OSSpinLock *__lock)
{
	lll_lock((OSLowLock*)__lock);
}

void OSSpinLockUnlock(volatile OSSpinLock *__lock)
{
	lll_unlock((OSLowLock*)__lock);
} 

// newline! \n

