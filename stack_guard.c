/*
 * core: stack_guard.c
 * Copyright (c) 2012 Christina Brooks
 *
 * 
 */

#include <stdio.h>
#include "OSLog.h"

#define	GUARD_MAX 8
long __stack_chk_guard[GUARD_MAX] = {0, 0, 0, 0, 0, 0, 0, 0};

void
__stack_chk_fail()
{
	OSHalt("__stack_chk_fail(): stack overflow?");
}

// newline! \n

