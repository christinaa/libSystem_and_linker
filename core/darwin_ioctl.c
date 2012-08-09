/*
 * core: darwin_ioctl.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Darwin-compatible IOCtl.
 */

/*
	I swear, I know what I'm doing.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/uio.h>
#include "OSLog.h"
#include <dirent.h>
#include <signal.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>

/* Shim Support */
#include "linux_ioctl.h"
#define ioctl_log(x, ...) OSLogLib("ioctl", x, ##__VA_ARGS__)

extern int ioctl$LINUX(int, int, void *);
#define platform_ioctl(x, y, z) ioctl$LINUX(x, y, z)

#pragma mark TermIOs

#define logEverything 0
//#define DEBUG
#define ldebug(x) 0

#define LINUX_NCCS 19
#define LINUX_NCC 8

struct linux_termio {
	unsigned short c_iflag;
	unsigned short c_oflag;
	unsigned short c_cflag;
	unsigned short c_lflag;
	unsigned char c_line;
	unsigned char c_cc[LINUX_NCC];
};

struct linux_termios {
	unsigned int c_iflag;
	unsigned int c_oflag;
	unsigned int c_cflag;
	unsigned int c_lflag;
	unsigned char c_line;
	unsigned char c_cc[LINUX_NCCS];
};

struct linux_winsize {
	unsigned short ws_row, ws_col;
	unsigned short ws_xpixel, ws_ypixel;
};

struct speedtab {
	int sp_speed;			/* Speed. */
	int sp_code;			/* Code. */
};

static struct speedtab sptab[] = {
	{ B0, LINUX_B0 }, { B50, LINUX_B50 },
	{ B75, LINUX_B75 }, { B110, LINUX_B110 },
	{ B134, LINUX_B134 }, { B150, LINUX_B150 },
	{ B200, LINUX_B200 }, { B300, LINUX_B300 },
	{ B600, LINUX_B600 }, { B1200, LINUX_B1200 },
	{ B1800, LINUX_B1800 }, { B2400, LINUX_B2400 },
	{ B4800, LINUX_B4800 }, { B9600, LINUX_B9600 },
	{ B19200, LINUX_B19200 }, { B38400, LINUX_B38400 },
	{ B57600, LINUX_B57600 }, { B115200, LINUX_B115200 },
	{-1, -1 }
};

static int
linux_to_bsd_speed(int code, struct speedtab *table)
{
	for ( ; table->sp_code != -1; table++)
		if (table->sp_code == code)
			return (table->sp_speed);
	return -1;
}

static int
bsd_to_linux_speed(int speed, struct speedtab *table)
{
	for ( ; table->sp_speed != -1; table++)
		if (table->sp_speed == speed)
			return (table->sp_code);
	return -1;
}

static void
bsd_to_linux_termios(struct termios *bios, struct linux_termios *lios)
{
	int i;
	
#ifdef DEBUG
	if (ldebug(ioctl)) {
		printf("LINUX: BSD termios structure (input):\n");
		printf("i=%08x o=%08x c=%08x l=%08x ispeed=%d ospeed=%d\n",
			   bios->c_iflag, bios->c_oflag, bios->c_cflag, bios->c_lflag,
			   bios->c_ispeed, bios->c_ospeed);
		printf("c_cc ");
		for (i=0; i<NCCS; i++)
			printf("%02x ", bios->c_cc[i]);
		printf("\n");
	}
#endif
	
	lios->c_iflag = 0;
	if (bios->c_iflag & IGNBRK)
		lios->c_iflag |= LINUX_IGNBRK;
	if (bios->c_iflag & BRKINT)
		lios->c_iflag |= LINUX_BRKINT;
	if (bios->c_iflag & IGNPAR)
		lios->c_iflag |= LINUX_IGNPAR;
	if (bios->c_iflag & PARMRK)
		lios->c_iflag |= LINUX_PARMRK;
	if (bios->c_iflag & INPCK)
		lios->c_iflag |= LINUX_INPCK;
	if (bios->c_iflag & ISTRIP)
		lios->c_iflag |= LINUX_ISTRIP;
	if (bios->c_iflag & INLCR)
		lios->c_iflag |= LINUX_INLCR;
	if (bios->c_iflag & IGNCR)
		lios->c_iflag |= LINUX_IGNCR;
	if (bios->c_iflag & ICRNL)
		lios->c_iflag |= LINUX_ICRNL;
	if (bios->c_iflag & IXON)
		lios->c_iflag |= LINUX_IXON;
	if (bios->c_iflag & IXANY)
		lios->c_iflag |= LINUX_IXANY;
	if (bios->c_iflag & IXOFF)
		lios->c_iflag |= LINUX_IXOFF;
	if (bios->c_iflag & IMAXBEL)
		lios->c_iflag |= LINUX_IMAXBEL;
	
	lios->c_oflag = 0;
	if (bios->c_oflag & OPOST)
		lios->c_oflag |= LINUX_OPOST;
	if (bios->c_oflag & ONLCR)
		lios->c_oflag |= LINUX_ONLCR;
	if (bios->c_oflag & TAB3)
		lios->c_oflag |= LINUX_XTABS;
	
	lios->c_cflag = bsd_to_linux_speed(bios->c_ispeed, sptab);
	lios->c_cflag |= (bios->c_cflag & CSIZE) >> 4;
	if (bios->c_cflag & CSTOPB)
		lios->c_cflag |= LINUX_CSTOPB;
	if (bios->c_cflag & CREAD)
		lios->c_cflag |= LINUX_CREAD;
	if (bios->c_cflag & PARENB)
		lios->c_cflag |= LINUX_PARENB;
	if (bios->c_cflag & PARODD)
		lios->c_cflag |= LINUX_PARODD;
	if (bios->c_cflag & HUPCL)
		lios->c_cflag |= LINUX_HUPCL;
	if (bios->c_cflag & CLOCAL)
		lios->c_cflag |= LINUX_CLOCAL;
	if (bios->c_cflag & CRTSCTS)
		lios->c_cflag |= LINUX_CRTSCTS;
	
	lios->c_lflag = 0;
	if (bios->c_lflag & ISIG)
		lios->c_lflag |= LINUX_ISIG;
	if (bios->c_lflag & ICANON)
		lios->c_lflag |= LINUX_ICANON;
	if (bios->c_lflag & ECHO)
		lios->c_lflag |= LINUX_ECHO;
	if (bios->c_lflag & ECHOE)
		lios->c_lflag |= LINUX_ECHOE;
	if (bios->c_lflag & ECHOK)
		lios->c_lflag |= LINUX_ECHOK;
	if (bios->c_lflag & ECHONL)
		lios->c_lflag |= LINUX_ECHONL;
	if (bios->c_lflag & NOFLSH)
		lios->c_lflag |= LINUX_NOFLSH;
	if (bios->c_lflag & TOSTOP)
		lios->c_lflag |= LINUX_TOSTOP;
	if (bios->c_lflag & ECHOCTL)
		lios->c_lflag |= LINUX_ECHOCTL;
	if (bios->c_lflag & ECHOPRT)
		lios->c_lflag |= LINUX_ECHOPRT;
	if (bios->c_lflag & ECHOKE)
		lios->c_lflag |= LINUX_ECHOKE;
	if (bios->c_lflag & FLUSHO)
		lios->c_lflag |= LINUX_FLUSHO;
	if (bios->c_lflag & PENDIN)
		lios->c_lflag |= LINUX_PENDIN;
	if (bios->c_lflag & IEXTEN)
		lios->c_lflag |= LINUX_IEXTEN;
	
	for (i=0; i<LINUX_NCCS; i++)
		lios->c_cc[i] = LINUX_POSIX_VDISABLE;
	lios->c_cc[LINUX_VINTR] = bios->c_cc[VINTR];
	lios->c_cc[LINUX_VQUIT] = bios->c_cc[VQUIT];
	lios->c_cc[LINUX_VERASE] = bios->c_cc[VERASE];
	lios->c_cc[LINUX_VKILL] = bios->c_cc[VKILL];
	lios->c_cc[LINUX_VEOF] = bios->c_cc[VEOF];
	lios->c_cc[LINUX_VEOL] = bios->c_cc[VEOL];
	lios->c_cc[LINUX_VMIN] = bios->c_cc[VMIN];
	lios->c_cc[LINUX_VTIME] = bios->c_cc[VTIME];
	lios->c_cc[LINUX_VEOL2] = bios->c_cc[VEOL2];
	lios->c_cc[LINUX_VSUSP] = bios->c_cc[VSUSP];
	lios->c_cc[LINUX_VSTART] = bios->c_cc[VSTART];
	lios->c_cc[LINUX_VSTOP] = bios->c_cc[VSTOP];
	lios->c_cc[LINUX_VREPRINT] = bios->c_cc[VREPRINT];
	lios->c_cc[LINUX_VDISCARD] = bios->c_cc[VDISCARD];
	lios->c_cc[LINUX_VWERASE] = bios->c_cc[VWERASE];
	lios->c_cc[LINUX_VLNEXT] = bios->c_cc[VLNEXT];
	
	for (i=0; i<LINUX_NCCS; i++) {
		if (i != LINUX_VMIN && i != LINUX_VTIME &&
		    lios->c_cc[i] == _POSIX_VDISABLE)
			lios->c_cc[i] = LINUX_POSIX_VDISABLE;
	}
	lios->c_line = 0;
	
#ifdef DEBUG
	if (ldebug(ioctl)) {
		printf("LINUX: LINUX termios structure (output):\n");
		printf("i=%08x o=%08x c=%08x l=%08x line=%d\n",
			   lios->c_iflag, lios->c_oflag, lios->c_cflag,
			   lios->c_lflag, (int)lios->c_line);
		printf("c_cc ");
		for (i=0; i<LINUX_NCCS; i++)
			printf("%02x ", lios->c_cc[i]);
		printf("\n");
	}
#endif
}

static void
linux_to_bsd_termios(struct linux_termios *lios, struct termios *bios)
{
	int i;
	
#ifdef DEBUG
	if (ldebug(ioctl)) {
		printf("LINUX: LINUX termios structure (input):\n");
		printf("i=%08x o=%08x c=%08x l=%08x line=%d\n",
			   lios->c_iflag, lios->c_oflag, lios->c_cflag,
			   lios->c_lflag, (int)lios->c_line);
		printf("c_cc ");
		for (i=0; i<LINUX_NCCS; i++)
			printf("%02x ", lios->c_cc[i]);
		printf("\n");
	}
#endif
	
	bios->c_iflag = 0;
	if (lios->c_iflag & LINUX_IGNBRK)
		bios->c_iflag |= IGNBRK;
	if (lios->c_iflag & LINUX_BRKINT)
		bios->c_iflag |= BRKINT;
	if (lios->c_iflag & LINUX_IGNPAR)
		bios->c_iflag |= IGNPAR;
	if (lios->c_iflag & LINUX_PARMRK)
		bios->c_iflag |= PARMRK;
	if (lios->c_iflag & LINUX_INPCK)
		bios->c_iflag |= INPCK;
	if (lios->c_iflag & LINUX_ISTRIP)
		bios->c_iflag |= ISTRIP;
	if (lios->c_iflag & LINUX_INLCR)
		bios->c_iflag |= INLCR;
	if (lios->c_iflag & LINUX_IGNCR)
		bios->c_iflag |= IGNCR;
	if (lios->c_iflag & LINUX_ICRNL)
		bios->c_iflag |= ICRNL;
	if (lios->c_iflag & LINUX_IXON)
		bios->c_iflag |= IXON;
	if (lios->c_iflag & LINUX_IXANY)
		bios->c_iflag |= IXANY;
	if (lios->c_iflag & LINUX_IXOFF)
		bios->c_iflag |= IXOFF;
	if (lios->c_iflag & LINUX_IMAXBEL)
		bios->c_iflag |= IMAXBEL;
	
	bios->c_oflag = 0;
	if (lios->c_oflag & LINUX_OPOST)
		bios->c_oflag |= OPOST;
	if (lios->c_oflag & LINUX_ONLCR)
		bios->c_oflag |= ONLCR;
	if (lios->c_oflag & LINUX_XTABS)
		bios->c_oflag |= TAB3;
	
	bios->c_cflag = (lios->c_cflag & LINUX_CSIZE) << 4;
	if (lios->c_cflag & LINUX_CSTOPB)
		bios->c_cflag |= CSTOPB;
	if (lios->c_cflag & LINUX_CREAD)
		bios->c_cflag |= CREAD;
	if (lios->c_cflag & LINUX_PARENB)
		bios->c_cflag |= PARENB;
	if (lios->c_cflag & LINUX_PARODD)
		bios->c_cflag |= PARODD;
	if (lios->c_cflag & LINUX_HUPCL)
		bios->c_cflag |= HUPCL;
	if (lios->c_cflag & LINUX_CLOCAL)
		bios->c_cflag |= CLOCAL;
	if (lios->c_cflag & LINUX_CRTSCTS)
		bios->c_cflag |= CRTSCTS;
	
	bios->c_lflag = 0;
	if (lios->c_lflag & LINUX_ISIG)
		bios->c_lflag |= ISIG;
	if (lios->c_lflag & LINUX_ICANON)
		bios->c_lflag |= ICANON;
	if (lios->c_lflag & LINUX_ECHO)
		bios->c_lflag |= ECHO;
	if (lios->c_lflag & LINUX_ECHOE)
		bios->c_lflag |= ECHOE;
	if (lios->c_lflag & LINUX_ECHOK)
		bios->c_lflag |= ECHOK;
	if (lios->c_lflag & LINUX_ECHONL)
		bios->c_lflag |= ECHONL;
	if (lios->c_lflag & LINUX_NOFLSH)
		bios->c_lflag |= NOFLSH;
	if (lios->c_lflag & LINUX_TOSTOP)
		bios->c_lflag |= TOSTOP;
	if (lios->c_lflag & LINUX_ECHOCTL)
		bios->c_lflag |= ECHOCTL;
	if (lios->c_lflag & LINUX_ECHOPRT)
		bios->c_lflag |= ECHOPRT;
	if (lios->c_lflag & LINUX_ECHOKE)
		bios->c_lflag |= ECHOKE;
	if (lios->c_lflag & LINUX_FLUSHO)
		bios->c_lflag |= FLUSHO;
	if (lios->c_lflag & LINUX_PENDIN)
		bios->c_lflag |= PENDIN;
	if (lios->c_lflag & LINUX_IEXTEN)
		bios->c_lflag |= IEXTEN;
	
	for (i=0; i<NCCS; i++)
		bios->c_cc[i] = _POSIX_VDISABLE;
	bios->c_cc[VINTR] = lios->c_cc[LINUX_VINTR];
	bios->c_cc[VQUIT] = lios->c_cc[LINUX_VQUIT];
	bios->c_cc[VERASE] = lios->c_cc[LINUX_VERASE];
	bios->c_cc[VKILL] = lios->c_cc[LINUX_VKILL];
	bios->c_cc[VEOF] = lios->c_cc[LINUX_VEOF];
	bios->c_cc[VEOL] = lios->c_cc[LINUX_VEOL];
	bios->c_cc[VMIN] = lios->c_cc[LINUX_VMIN];
	bios->c_cc[VTIME] = lios->c_cc[LINUX_VTIME];
	bios->c_cc[VEOL2] = lios->c_cc[LINUX_VEOL2];
	bios->c_cc[VSUSP] = lios->c_cc[LINUX_VSUSP];
	bios->c_cc[VSTART] = lios->c_cc[LINUX_VSTART];
	bios->c_cc[VSTOP] = lios->c_cc[LINUX_VSTOP];
	bios->c_cc[VREPRINT] = lios->c_cc[LINUX_VREPRINT];
	bios->c_cc[VDISCARD] = lios->c_cc[LINUX_VDISCARD];
	bios->c_cc[VWERASE] = lios->c_cc[LINUX_VWERASE];
	bios->c_cc[VLNEXT] = lios->c_cc[LINUX_VLNEXT];
	
	for (i=0; i<NCCS; i++) {
		if (i != VMIN && i != VTIME &&
		    bios->c_cc[i] == LINUX_POSIX_VDISABLE)
			bios->c_cc[i] = _POSIX_VDISABLE;
	}
	
	bios->c_ispeed = bios->c_ospeed =
	linux_to_bsd_speed(lios->c_cflag & LINUX_CBAUD, sptab);
	
#ifdef DEBUG
	if (ldebug(ioctl)) {
		printf("LINUX: BSD termios structure (output):\n");
		printf("i=%08x o=%08x c=%08x l=%08x ispeed=%d ospeed=%d\n",
			   bios->c_iflag, bios->c_oflag, bios->c_cflag, bios->c_lflag,
			   bios->c_ispeed, bios->c_ospeed);
		printf("c_cc ");
		for (i=0; i<NCCS; i++)
			printf("%02x ", bios->c_cc[i]);
		printf("\n");
	}
#endif
}

static void
bsd_to_linux_termio(struct termios *bios, struct linux_termio *lio)
{
	struct linux_termios lios;
	
	bsd_to_linux_termios(bios, &lios);
	lio->c_iflag = lios.c_iflag;
	lio->c_oflag = lios.c_oflag;
	lio->c_cflag = lios.c_cflag;
	lio->c_lflag = lios.c_lflag;
	lio->c_line  = lios.c_line;
	memcpy(lio->c_cc, lios.c_cc, LINUX_NCC);
}

static void
linux_to_bsd_termio(struct linux_termio *lio, struct termios *bios)
{
	struct linux_termios lios;
	int i;
	
	lios.c_iflag = lio->c_iflag;
	lios.c_oflag = lio->c_oflag;
	lios.c_cflag = lio->c_cflag;
	lios.c_lflag = lio->c_lflag;
	for (i=LINUX_NCC; i<LINUX_NCCS; i++)
		lios.c_cc[i] = LINUX_POSIX_VDISABLE;
	memcpy(lios.c_cc, lio->c_cc, LINUX_NCC);
	linux_to_bsd_termios(&lios, bios);
}

#define BSD_TERMIO_MIN (TIOCMODG & 0xffff)
#define BSD_TERMIO_MAX (TIOCSBRK & 0xffff)

int ioctl$BRIDGE$termios(int fd, unsigned long request, void* param)
{
	struct termios bios;
	struct linux_termios lios;
	struct linux_termio lio;
	struct file *fp;
	int error;
	
	//OSLog("ioctl$BRIDGE$termios");
	
	switch (request) {
		case TIOCGETA:
		{
			if (logEverything) {
				ioctl_log("termios: conv 'TIOCGETA'");
			}
			
			error = platform_ioctl(fd, LINUX_TCGETS, &lios);
			if (error) {
				break;
			}
			
			linux_to_bsd_termios(&lios, (struct termios*)param);
			
			break;
		}

		case TIOCSETA:
		{
			if (logEverything) {
				ioctl_log("termios: conv 'TIOCSETA'");
			}
			
			bsd_to_linux_termios((struct termios*)param, &lios);
			
			error = platform_ioctl(fd, LINUX_TCSETS, &lios);
			if (error) {
				break;
			}

			break;
		}
			
		case TIOCSETAW:
		{
			if (logEverything) {
				ioctl_log("termios: conv 'TIOCSETAW'");
			}
			
			bsd_to_linux_termios((struct termios*)param, &lios);
			
			error = platform_ioctl(fd, LINUX_TCSETSW, &lios);
			if (error) {
				break;
			}
			
			break;
		}
		
#define failCase(x) case x: ioctl_log("termios: '%s' unimpl", #x); error = -1; break;			

			failCase(TIOCSTOP);
			failCase(TIOCSTART);
			failCase(TIOCSTAT);
			
			//failCase(T);
			
		case TIOCSETAF:
		{
			if (logEverything) {
				ioctl_log("termios: conv 'TIOCSETAF'");
			}
			
			bsd_to_linux_termios((struct termios*)param, &lios);
			
			error = platform_ioctl(fd, LINUX_TCSETSF, &lios);
			if (error) {
				break;
			}
			
			break;
		}

#define directCase(x) case x: if (logEverything) \
ioctl_log("termios: '%s' no conv", #x);\
error = platform_ioctl(fd, LINUX_## x, param); break;
			
			directCase(TIOCEXCL);
			directCase(TIOCGPGRP);
			directCase(TIOCNXCL);
			directCase(TIOCSCTTY);
			directCase(TIOCGWINSZ);
			directCase(TIOCSWINSZ);
			directCase(TIOCSPGRP);
			directCase(TIOCMGET);
			directCase(TIOCMBIS);
			directCase(TIOCMBIC);
			directCase(TIOCMSET);
			directCase(FIONREAD);
			directCase(TIOCCONS);
			directCase(TIOCPKT);
			directCase(FIONBIO);
			directCase(FIONCLEX);
			directCase(FIOASYNC);
			directCase(TIOCSBRK);
			directCase(TIOCCBRK);

			
		case TCIOFLUSH:
		{
			if (logEverything) {
				ioctl_log("termios: unimpl TCIOFLUSH");
			}
			break;
		}
			
		default:
		{
			if (logEverything) {
				ioctl_log("termios: unk BSD request (%p)", request);
			}
			break;
		}
	}
	
	if (logEverything)
		OSLog("termios: error = %d", error);
	
	return error;
}

int ioctl$darwin(int fd, unsigned long request, void* param) {
	int cmd = request & 0xffff;

	if (logEverything) {
		ioctl_log("ioct$xxx(%d, %p, %p): {cmd=%p}",
			  fd,
			  (void*)request,
			  param,
			  (void*)cmd);
	}
	
	if (cmd >= BSD_TERMIO_MIN && cmd <= BSD_TERMIO_MAX) {
		return ioctl$BRIDGE$termios(fd, request, param);
	}
	else {
		if (logEverything) {
			ioctl_log("ioct$xxx(%d, %p, %p): falling through", fd, request, param);
		}
		
		return platform_ioctl(fd, request, param);
	}
}


