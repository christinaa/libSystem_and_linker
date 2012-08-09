/*
 * Core Framework: Libc: signal.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Signal stuff.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/uio.h>
#include "OSLog.h"
#include <dirent.h>

#define verboseSignalLog 0

#define sig_trace() OSLogLib("sig", "TRACE: [%s:%d]", __FUNCTION__, __LINE__)
#define sig_log(x, ...) OSLogLib("sig", x, ##__VA_ARGS__)

/*
	New syscalls.
 */
int sigaction$native(int act, struct sigaction * rs, struct sigaction * rd);
int sigprocmask$native(int how, const sigset_t *set, sigset_t *oldset);

int	sigaction(int act, const struct sigaction * __restrict rs, struct sigaction * __restrict rd)
{
	return sigaction$native(act, ( struct sigaction *)rs, rd);
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
	return sigprocmask$native(how, set, oldset);
}

typedef void (*sig_t) (int);

sig_t
signal(int sig, sig_t func)
{
	sig_log("signal(%d, %p)", sig, func);
	return NULL;
}

int
sigsetmask(int mask)
{
	int  n;
	
	union {
		int       the_mask;
		sigset_t  the_sigset;
	} in, out;
	
	sigemptyset(&in.the_sigset);
	in.the_mask = mask;
	
	n = sigprocmask(SIG_SETMASK, &in.the_sigset, &out.the_sigset);
	if (n)
		return n;
	
	return out.the_mask;
}

extern pid_t gettid(void);
int raise(int signum)
{
    return kill(gettid(), signum);
}