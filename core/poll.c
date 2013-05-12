/*
 * core: poll.c
 * Copyright (c) 2012 Christina Brooks
 *
 * 
 */

#include <stdio.h>
#include <poll.h>
#include "OSLog.h"

int
poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
	OSHalt("poll(): mary had a little lamb, little lamb. "
		   "AND THEN IT DIED. JUST LIKE THIS FUNCTION. "
		   "(just kidding, it's not implemented. "
		   "but the little lamb actually died)");
}

// newline! \n

