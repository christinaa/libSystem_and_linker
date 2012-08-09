/*
 * Core Framework: semaphore.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Semaphores or some shit, I don't know anymore.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "OSLog.h"
#include <errno.h>
#include <libkern/OSAtomic.h>
#include <semaphore.h>
#include <mach/semaphore.h>

#define OSSemaphoreNotImplemented() OSLog("%s: foo", __FUNCTION__);

void __reserved_semaphore_1() {
	
	OSHalt("called a reserved function");
}

kern_return_t semaphore_create4(task_t task, semaphore_t *semaphore, int policy, int value)
{
	OSLog("semaphore_create(%d, %p, %d, %d)", task, semaphore, policy, value);
	return KERN_SUCCESS;
}

kern_return_t semaphore_wait4(semaphore_t semaphore)
{
	OSLog("semaphore_wait(%d)", semaphore);
	return KERN_SUCCESS;
}

kern_return_t semaphore_signal4(semaphore_t semaphore) 
{
	OSSemaphoreNotImplemented();
	return KERN_SUCCESS;
}

kern_return_t semaphore_signal_all4(semaphore_t semaphore)
{
	OSSemaphoreNotImplemented();
	return KERN_SUCCESS;
}

kern_return_t semaphore_timedwait4(semaphore_t semaphore, mach_timespec_t wait_time)
{
	OSSemaphoreNotImplemented();
	return KERN_SUCCESS;
}

kern_return_t semaphore_timedwait_signal4(semaphore_t wait_semaphore, semaphore_t signal_semaphore, mach_timespec_t wait_time)
{
	OSSemaphoreNotImplemented();
	return KERN_SUCCESS;
}

kern_return_t semaphore_wait_signal4(semaphore_t wait_semaphore, semaphore_t signal_semaphore)
{
	OSSemaphoreNotImplemented();
	return KERN_SUCCESS;
}

kern_return_t semaphore_signal_thread4(semaphore_t semaphore, thread_t thread)
{
	OSSemaphoreNotImplemented();
	return KERN_SUCCESS;
}
