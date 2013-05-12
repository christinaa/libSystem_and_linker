/*
 * Core Framework: Mach Time
 * Copyright (c) 2012 Christina Brooks
 *
 * No idea...
 */

#include <stdio.h>
#include "_lnk_log_c.h"
#include <mach/message.h>
#include <stdlib.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <pthread.h>
#include "OSLog.h"

/*
	Timebase
 */
kern_return_t mach_timebase_info(struct mach_timebase_info* info)
{
	info->numer = 1;
	info->denom = CLOCKS_PER_SEC;
	
	return KERN_SUCCESS;
}

uint64_t mach_absolute_time(void)
{
	return 0;
}

// newline! \n

