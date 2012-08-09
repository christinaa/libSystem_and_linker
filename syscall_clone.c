/*
 * Core Framework: LibSyscall: cloneShim.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Higher level clone implementation.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include "_lnk_log_c.h"
#include "LX_sched.h"

/* Syscall */
extern int  clone$SYS$LINUX(unsigned long   clone_flags,
						   void*           newsp,
						   int            *parent_tidptr,
						   void           *new_tls,
						   int            *child_tidptr,
						   int            (*fn)(void *),
						   void          *arg);


int
clone$LINUX(int (*fn)(void *), void *child_stack, int flags, void*  arg, ...)
{
    va_list  args;
    int     *parent_tidptr = NULL;
    void    *new_tls = NULL;
    int     *child_tidptr = NULL;
    int     ret;
	
    /* extract optional parameters - they are cummulative */
    va_start(args, arg);
    if (flags & (LX_CLONE_PARENT_SETTID| LX_CLONE_SETTLS| LX_CLONE_CHILD_SETTID)) {
        parent_tidptr = va_arg(args, int*);
    }
    if (flags & (LX_CLONE_SETTLS | LX_CLONE_CHILD_SETTID)) {
        new_tls = va_arg(args, void*);
    }
    if (flags & LX_CLONE_CHILD_SETTID) {
        child_tidptr = va_arg(args, int*);
    }
    va_end(args);

    ret = clone$SYS$LINUX(flags, child_stack, parent_tidptr, new_tls, child_tidptr, fn, arg);
    return ret;
}
