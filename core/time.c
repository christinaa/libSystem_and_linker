/*
 * Core Framework: Libc: time.c
 * Copyright (c) 2012 Christina Brooks
 *
 * What time is it?
 * WAT TAAM IZ IT??!!!
 */

#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <sys/uio.h>

#include <time.h>
#include "OSLog.h"

/* Linux has 64/64 while Darwin has 32/64 */
typedef struct {
	long tv_sec;		/* seconds */
	long tv_usec;	/* and microseconds */
} linux_timeval_t;

//char* tzname[] = {"UTC"};
static clock_t clock_start;
extern int gettimeofday$LINUX(linux_timeval_t *tv, struct timezone *tz);
int clock_gettime$LINUX(int clk_id, struct timespec *tp);

void __reserved_time_1(void) {
	OSHalt("__reserved_time_1");
}

#define CLOCK_MONOTONIC                 1

size_t _UNIMP__strftime(char *s, size_t maxsize, const char *format,
				const struct tm *timptr)
{
	if (maxsize > 3) {
		s[0] = 'T';
		s[1] = 'M';
		s[2] = 0;
		
		//OSLog("strftime(%s, %d, %s)", s, maxsize, format);
		
		return 2;
	}
	
	return 0;
}

static inline clock_t clock_now(void)
{
	struct timespec tm;
	clock_gettime$LINUX( CLOCK_MONOTONIC, &tm);
	return tm.tv_sec * CLOCKS_PER_SEC + (tm.tv_nsec * (CLOCKS_PER_SEC/1e9));
}

void _time_init(void) {
	clock_start = clock_now();
}

int
gettimeofday(struct timeval *restrict tp, void *restrict tzp)
{
	//OSLogLib("time", "gettimeofday(%p, %p)", tp, tzp);
	
	linux_timeval_t timeval;
	int ret = gettimeofday$LINUX(&timeval, tzp);
	
	tp->tv_sec = timeval.tv_sec;
	tp->tv_usec = (int)timeval.tv_usec;
	
	return ret;
}

time_t
time(time_t *t)
{
	struct timeval tt;
	
	if (gettimeofday(&tt, (struct timezone *)0) < 0)
		return (-1);
	if (t)
		*t = (time_t)tt.tv_sec;
	return (tt.tv_sec);
}


clock_t
clock(void)
{
	/*
		XXX:
		bionic said something about threads. i need to look into
		that. see /unistd/time.c
	 */
	return clock_now() - clock_start;
}

void _UNIMP__tzset(void) {
	/*
		XXX
	 */
	OSLog("tzset()");
}

// newline! \n

