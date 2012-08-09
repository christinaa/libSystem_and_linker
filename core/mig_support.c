/*
 * core: mig_support.c
 * Copyright (c) 2012 Christina Brooks
 *
 * 
 */

/*
	Can't be bothered right now.
 */

#include <stdio.h>
#include <mach/mach.h>
#include "OSLog.h"

extern mach_port_t mach_reply_port(void);

void
mig_put_reply_port(mach_port_t	reply_port)
{
	OSHalt("mig_put_reply_port");
}

void
mig_dealloc_reply_port(mach_port_t migport)
{
	OSLog("mig_dealloc_reply_port NOT IMPLEMENTED");
}

mach_port_t
mig_get_reply_port()
{
	return mach_reply_port();
}

void
mig_init(int init_done)
{
	OSLog(" *** mig_init(%d) ***", init_done);
}

void mig_fork_child()
{
	OSHalt("mig_fork_child");
}
