/*
 * core: darwin_pspawn.c
 * Copyright (c) 2012 Christina Brooks
 *
 * 
 */

#include <stdio.h>
#include <spawn.h>
#include "OSLog.h"

int __posix_spawn(pid_t * __restrict pid,
				  const char * __restrict path,
				  void * args,
				  char *const argv[ __restrict],
				  char *const envp[ __restrict])
{
	OSHalt("__posix_spawn: Not implemented.");
}
