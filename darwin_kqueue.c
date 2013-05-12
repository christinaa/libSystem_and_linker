/*
 * core: darwin_kqueue.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Something like BSD kernel queues. 
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include "OSLog.h"

typedef union epoll_data {
	void    *ptr;
	int      fd;
	uint32_t u32;
	uint64_t u64;
} epoll_data_t;

struct epoll_event {
	uint32_t     events;    /* Epoll events */
	epoll_data_t data;      /* User data variable */
};

int epoll_create$LINUX(int size);
int epoll_wait$LINUX(int epfd, struct epoll_event *events, int maxevents, int timeout);
int epoll_ctl$LINUX(int epfd, int op, int fd, struct epoll_event *event);

int
kqueue(void)
{
	/*
		The kqueue identifier is opaque, so lets just return
		the epoll's descriptor.
	 */
	return epoll_create$LINUX(0);
}

int
kevent(int kq,
	   const struct kevent *changelist,
	   int nchanges,
	   struct kevent *eventlist,
	   int nevents,
	   const struct timespec *timeout)
{
	return -1;
}

int
kevent64(int kq,
		 const struct kevent64_s *changelist,
		 int nchanges,
		 struct kevent64_s *eventlist,
		 int nevents,
		 unsigned int flags,
         const struct timespec *timeout)
{
	OSHalt("kevent64: why would you do this?");
}

// newline! \n

