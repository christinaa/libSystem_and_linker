/*
 * Core Framework: Libc: unistd.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Unistd interface implementation. The majority of unistd
 * resides here.
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

/* Shim Support */
#include "LX_fcntl.h"

#define unistd_log(x, ...) OSLogLib("unistd", x, ##__VA_ARGS__)

extern char* gArgv[1024];

/* Syscalls */
extern int execve$LINUX(const char *filename, char *const argv[], char *const envp[]);
extern int fcntl64$LINUX(int, int, void *);
extern int fork$LINUX(void);
extern int getcwd$LINUX(char * buf, size_t size);
extern int llseek$LINUX(int fd, unsigned long  offset_hi, unsigned long offset_lo, off_t*  result, int  whence);
extern int pread64$LINUX(int fd, void *buf, size_t nbytes, long long offset);
extern int open$LINUX(const char*, int, int);
extern __attribute__ ((noreturn)) void exit_group$LINUX(int); /* shut up gcc */
extern ssize_t writev$LINUX(int fd, const struct iovec *iov, int iovcnt);
extern ssize_t readv$LINUX(int fd, const struct iovec *iov, int iovcnt);
extern int ioctl$LINUX(int, int, void *);
extern int getgroups$LINUX(int, gid_t []);
extern gid_t getgid$LINUX(void);
extern pid_t wait4$LINUX(pid_t pid, int *status, int options, struct rusage *rusage);

#define convertFlag(old, new) if (flags & old) {\
uflags |= new; }

#define LX_WNOHANG         0x00000001
#define LX_WUNTRACED       0x00000002
#define LX_WSTOPPED        LX_WUNTRACED
#define LX_WEXITED         0x00000004
#define LX_WCONTINUED      0x00000008
#define LX_WNOWAIT         0x01000000      /* Don't reap, just poll status.  */
 
#define LX___WNOTHREAD     0x20000000      /* Don't wait on children of other threads in this group */
#define LX___WALL          0x40000000      /* Wait on all children, regardless of type */
#define LX___WCLONE        0x80000000      /* Wait only on non-SIGCHLD children */

extern int ioctl$darwin(int fd, unsigned long request, void* param);

const char *getprogname(void) {
	if (gArgv[0] != NULL) {
		return gArgv[0];
	}
	else {
		return "unknown";
	}
}

/*******************************************************************
 ioctl -- control device
 */
int ioctl(int fd, unsigned long request, ...)
{
	/*
	 Library friendly va_args version
	 No idea why they use va_args, honestly.
	 */
	
	va_list ap;
	void * arg;
	
	va_start(ap, request);
	arg = va_arg(ap, void *);
	va_end(ap);
	
	/* Guts */
	return ioctl$darwin(fd, request, arg);
}


/*******************************************************************
 libc config
 */
size_t confstr(int name, char *buf, size_t len)
{
	unistd_log("confstr not implemented");
	
	return 0;
}


/*******************************************************************
 wait family
 */
pid_t wait( int*  status )
{
	unistd_log("wait");
	
	return wait4$LINUX( (pid_t)-1, status, 0, NULL );
}

pid_t wait3(int*  status, int options, struct rusage*  rusage)
{
	unistd_log("wait3");
	
	return wait4$LINUX( (pid_t)-1, status, options, rusage );
}

pid_t waitpid(pid_t  pid, int*  status, int  flags)
{
	int uflags = 0;
	
	convertFlag(WNOHANG, LX_WNOHANG);
	convertFlag(WUNTRACED, LX_WUNTRACED);
	convertFlag(WCONTINUED, LX_WCONTINUED);
	convertFlag(WNOWAIT, LX_WNOWAIT);
	
    int ret = wait4$LINUX(pid, status, uflags, NULL);

	unistd_log("waitpid(%d, %p, %d): status = %p, ret = %d", pid, status, uflags, *status, ret);
	
	return ret;
}

int reboot(int v)
{
	OSHalt("lo, reboot!");
}

int getdtablesize()
{
	unistd_log("getdtablesize(): not implemented, returning 20");
	
	return 20;
}

int gethostname(char*  buff, size_t  buflen)
{
	unistd_log("gethostname(%p, %d)", buff, buflen);
	
	const char* name = "hostname";
	
    if (1)
    {
        int  namelen = strlen(name);
		
        if ((int)buflen < namelen+1) {
			return -1;
		}
		else {
			memcpy(buff, name, namelen+1);
		}
	}
	
	return 0;
}

long
sysconf(int name)
{
	unistd_log("sysconf(%d): xxx", name);
	
	switch (name) {
		case _SC_OPEN_MAX:
			return 20;
			break;
		case _SC_CHILD_MAX:
			return 20;
			break;
		case _SC_NGROUPS_MAX:
			return 20;
			break;
		default:
			break;
	}
	
	
	/* meh */
	unistd_log("sysconf(%d): unimplemented flag", name);
	
	return 0;
}

int
getrusage(int who, struct rusage *r_usage)
{
	OSHalt("getrusage() not implemented!");
}

int
getrlimit(int resource, struct rlimit *rlp)
{
	OSHalt("getrlimit() not implemented!");
}

int
setrlimit(int resource, const struct rlimit *rlp)
{
	OSHalt("setrlimit() not implemented!");
}

/*******************************************************************
 killpg
 */
int
killpg(pid_t pgid, int sig)
{
	if (pgid == 1) {
		errno = ESRCH;
		return (-1);
	}
	return (kill(-pgid, sig));
}

int setuid(uid_t u)
{
	/* XXX */
	return 0;
}

gid_t getegid(void)
{
	return getgid();
} 

int getgroups(int dd, gid_t * gr)
{
	return getgroups$LINUX(dd, gr);
}

/*******************************************************************
	exit stuff
 */
void
__attribute__ ((noreturn))
_exit(int code) {
	exit_group$LINUX(code);
}

void exit(int code) {
	_exit(code);
}

/*******************************************************************
	fork -- create a new process
 */
int fork(void)
{
	int ret;
	
	ret = fork$LINUX();
	
	//unistd_log("fork: pid = %d", ret);
	
	if (ret != 0) {
		/* Parent */
		
	}
	else {
		/* Child */
	}
	
	return ret;
}




/*******************************************************************
 n_ioctl -- control device
 */
int n_ioctl(int fd, int request, ...)
{
	OSHalt("n_ioctl: obsolete function, do not call!");
	
    va_list ap;
    void * arg;
	
    va_start(ap, request);
    arg = va_arg(ap, void *);
    va_end(ap);
	
    return ioctl$LINUX(fd, request, arg);
}


/*******************************************************************
	writev
	XXX: different ABI?
 */
ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
	OSHalt("writev (unistd.c)");
	
	return writev$LINUX(fd, iov, iovcnt);
}


/*******************************************************************
	readv
	XXX: different ABI?
 */
ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
	OSHalt("readv (unistd.c)");
	
	return readv(fd, iov, iovcnt);
}


/*******************************************************************
	execve
 */
int execve(const char *path, char *const argv[], char *const envp[])
{
	//unistd_log("execve %s", path);
	
	/* this should work */
	return execve$LINUX(path, argv, envp);
}


/*******************************************************************
	usleep
	You know what's nice? timespec is the same on Darwin and Linux
 */
int usleep(useconds_t usec)
{
	struct timespec ts;
	
	ts.tv_sec  = usec/1000000UL;
	
#ifdef __arm__
    /* avoid divisions and modulos on the ARM */
	ts.tv_nsec = (usec - ts.tv_sec*1000000UL)*1000;
#else
	ts.tv_nsec = (usec % 1000000UL) * 1000UL;
#endif
	
	for (;;)
	{
		if ( nanosleep( &ts, &ts ) == 0 )
			return 0;
		
		// We try again if the nanosleep failure is EINTR.
		// The other possible failures are EINVAL (which we should pass through),
		// and ENOSYS, which doesn't happen.
		if ( errno != EINTR )
			return -1;
	}
}


/*******************************************************************
	sleep
 */
unsigned int sleep(unsigned int seconds)
{
    struct timespec  t;
	
	/* seconds is unsigned, while t.tv_sec is signed
	 * some people want to do sleep(UINT_MAX), so fake
	 * support for it by only sleeping 2 billion seconds
	 */
    if ((int)seconds < 0)
        seconds = 0x7fffffff;
	
    t.tv_sec  = seconds;
    t.tv_nsec = 0;
	
    if ( !nanosleep( &t, &t ) )
		return 0;
	
    if ( errno == EINTR )
        return t.tv_sec;
	
    return -1;
}


/*******************************************************************
	fcntl -- file control
	WARNING *** The ABI is probably incompatible.
 */
int fcntl(int fd, int cmd, ...)
{
    va_list ap;
    void * arg;
	
    va_start(ap, cmd);
    arg = va_arg(ap, void *);
    va_end(ap);
	
    return fcntl64$LINUX(fd, cmd, arg);
}

ssize_t pread(int fd, void *buf, size_t nbytes, off_t offset)
{
	unistd_log("pread");
	
    return pread64$LINUX(fd, buf, nbytes, offset);
}


/*******************************************************************
	ftok -- create IPC identifier from path name
 */
key_t  ftok(const char*  path, int  id)
{
    struct stat   st;
	
    if ( lstat(path, &st) < 0 )
        return -1;
	
    return (key_t)( (st.st_ino & 0xffff) | ((st.st_dev & 0xff) << 16) | ((id & 255) << 24) );
}


/*******************************************************************
	getcwd, getwd -- get working directory pathname
 */
char *getcwd(char *buf, size_t size)
{
	/* posix extension */
	if (buf == NULL) {
		void* rbuf = malloc(PATH_MAX);
		getcwd$LINUX(rbuf, PATH_MAX);
		return rbuf;
	}
	else {
		unistd_log("getcwd: non posix extension version");
		
		getcwd$LINUX(buf, size);
		return NULL;
	}
}


/*******************************************************************
	lseek -- reposition read/write file offset
 */
off_t lseek64(int fd, off_t off, int whence)
{
    off_t  result;
	
    if (llseek$LINUX(fd, (unsigned long)(off >> 32),(unsigned long)(off), &result, whence) < 0 )
        return -1;
	
    return result;
}


int open(const char *pathname, int flags, ...)
{
    mode_t mode = 0;
	int uflags = 0;
	
	//OSLog("OPEN: %s", pathname);
	
    uflags |= LX_O_LARGEFILE;
	
	/*
		A painful process of converting Darwin flags to Linux flags.
	 */
	convertFlag(O_CREAT, LX_O_CREAT);
	convertFlag(O_ACCMODE, LX_O_ACCMODE);
	convertFlag(O_EXCL, LX_O_EXCL);
	convertFlag(O_TRUNC, LX_O_TRUNC);
	convertFlag(O_RDWR, LX_O_RDWR);
	convertFlag(O_NONBLOCK, LX_O_NONBLOCK);
	convertFlag(O_DIRECTORY, LX_O_DIRECTORY);
	convertFlag(O_ASYNC, LX_FASYNC);
	convertFlag(O_RDONLY, LX_O_RDONLY);
	convertFlag(O_WRONLY, LX_O_WRONLY);
	convertFlag(O_SYNC, LX_O_SYNC);
	convertFlag(O_APPEND, LX_O_APPEND);
	convertFlag(O_NOFOLLOW, LX_O_NOFOLLOW);
	
    if (uflags & O_CREAT)
    {
        va_list  args;
		
        va_start(args, flags);
        mode = (mode_t) va_arg(args, int);
        va_end(args);
    }
	
    return open$LINUX(pathname, uflags, mode);
}

