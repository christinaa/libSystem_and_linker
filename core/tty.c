/*
 * Core Framework: Libc: tty.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Unistd ttys.
 */



#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/uio.h>
#include "OSLog.h"
#include <dirent.h>
#include <signal.h>
#include <setjmp.h>
#include <termios.h>
#include <sys/ioctl.h>

#define tty_log(x, ...) /* OSLogLib("tty", x, ##__VA_ARGS__) */



speed_t	cfgetispeed(const struct termios *s) {
	OSHalt("cfgetispeed");
}

speed_t	cfgetospeed(const struct termios *s) {
	OSHalt("cfgetospeed");
}

int	tcgetattr(int fd, struct termios *s)
{
	tty_log("tcgetattr(%d, %p)", fd, s);
	
	return ioctl(fd, TIOCGETA, (void *)s);
}

int	tcsetattr(int fd, int opt, const struct termios *t)
{
	tty_log("tcsetattr(%d, %d, %p)", fd, opt, s);
	
	/*
		FreeBSD implementation.
	 */
	
	struct termios localterm;
	
	if (opt & TCSASOFT) {
		localterm = *t;
		localterm.c_cflag |= CIGNORE;
		t = &localterm;
	}
	switch (opt & ~TCSASOFT) {
		case TCSANOW:
			return (ioctl(fd, TIOCSETA, t));
		case TCSADRAIN:
			return (ioctl(fd, TIOCSETAW, t));
		case TCSAFLUSH:
			return (ioctl(fd, TIOCSETAF, t));
		default:
			errno = EINVAL;
			return (-1);
	}
}

int	tcflow(int a, int b)
{
	OSHalt("tcflow: goof");
}

int	tcflush(int fd, int q) {
	tty_log("tcflush(%d, %d)", fd, q);
	
	return ioctl(fd, TIOCFLUSH, (void*)q);
}

pid_t tcgetpgrp(int fd)
{
	tty_log("tcgetpgrp(%d)", fd);
	
    pid_t _pid;
    return ioctl(fd, TIOCGPGRP, &_pid) ? (pid_t)-1 : _pid;
}

int tcsetpgrp(int fd, pid_t _pid)
{
	tty_log("tcsetpgrp(%d, %d)", fd, _pid);
	
    return ioctl(fd, TIOCSPGRP, &_pid);
}

int
isatty(int fd)
{
	tty_log("isatty(%d)", fd);
	
	struct termios term;
	return tcgetattr(fd, &term) == 0;
}

char *
ttyname(int fildes)
{
	tty_log("ttyname(%d)", fildes);
	
	return "g";
}

int
ttyslot(void) {
	OSHalt("go fuck yourself");
}


