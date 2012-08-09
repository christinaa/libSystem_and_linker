/*
 * core: darwin_socket.c
 * Copyright (c) 2012 Christina Brooks
 *
 * New native socket API.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "OSLog.h"
#include <sys/socket.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define ns_log(x, ...) OSLogLib("native_sock", x, ##__VA_ARGS__)
#define ns_not_implemented() OSHalt("native_sock: (%s:%d) not implemented ", __FUNCTION__, __LINE__)
#define ns_trace() OSLogLib("native_sock", "trace (%s:%d)", __FUNCTION__, __LINE__)
#define ns_error_trace() OSLogLib("native_sock", "error (%s:%d)", __FUNCTION__, __LINE__)

/*
 * System calls.
 */
extern int getpeername$native(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern int socket$native(int domain, int type, int protocol);
extern ssize_t send$native(int sockfd, const void *buf, size_t len, int flags);
extern ssize_t sendto$native(int sockfd, const void *buf, size_t len, int flags, struct sockaddr *dest_addr, socklen_t addrlen);
extern ssize_t recv$native(int sockfd, void *buf, size_t len, int flags);
extern ssize_t recvfrom$native(int sockfd, void *buf, size_t len, int flags, struct sockaddr  *src_addr, socklen_t *addrlen);
extern int listen$native(int sockfd, int backlog);
extern int getsockopt$native(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
extern int setsockopt$native(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
extern int bind$native(int sockfd, struct sockaddr *addr, socklen_t addrlen);
extern int connect$native(int sockfd, struct sockaddr *addr, socklen_t addrlen);
extern int accept$native(int sockfd, struct sockaddr *addr, socklen_t *addrlen);


/*
 * Implementations. 
 */

int	socket(int domain, int type, int protocol)
{
	return socket$native(domain, type, protocol);
}

int
getaddrinfo(const char *hostname, const char *servname, const struct addrinfo *hints, struct addrinfo **res)
{
	ns_log("getaddrinfo(%s, %s)", hostname, servname);
	return 0;
}

void
freeaddrinfo(struct addrinfo *ai)
{
	ns_not_implemented();
}

const char *
gai_strerror(int ecode)
{
	/* Can't be fucked to implement this right now. */
	ns_log("gai_strerror(%d)", ecode);
	return "wtf";
}

void __reserved_native_sock_1(void) {
	ns_not_implemented();
}

int	accept(int a, struct sockaddr *b , socklen_t * x)
{
	return accept$native(a, b, x);
}

int	bind(int a, const struct sockaddr * b, socklen_t c) 
{
	return bind$native(a, (struct sockaddr*)b, c);
}

int	connect(int a, const struct sockaddr * b, socklen_t c)
{
	return connect$native(a, (struct sockaddr*)b, c);
}

int	getpeername(int a, struct sockaddr *b , socklen_t * c)
{
	return getpeername$native(a, b, c);
}

int	getsockname(int a, struct sockaddr *b, socklen_t *c)
{
	ns_not_implemented(); /* [XXX][Not a system call] */
}

int	getsockopt(int a, int b, int c, void *d, socklen_t *e)
{
	return getsockopt$native(a, b, c, d, e);
}

int	listen(int a, int b) 
{
	return listen$native(a, b);
}

ssize_t	recv(int a, void *b, size_t c, int d) 
{
	return recv$native(a, b, c, d);
}

ssize_t	recvfrom(int a, void *b, size_t c, int d, struct sockaddr *e , socklen_t *f)
{
	return recvfrom$native(a, b, c, d, e, f);
}

ssize_t	recvmsg(int a, struct msghdr * b, int c)
{
	ns_not_implemented(); /* [XXX][Not a system call] */
}

ssize_t	send(int a, const void *b, size_t c, int d)
{
	return send$native(a, b, c, d);
}

ssize_t	sendmsg(int a, const struct msghdr *b, int c)
{
	ns_not_implemented(); /* [XXX][Not a system call] */
}

ssize_t	sendto(int a, const void *b, size_t c, int d, const struct sockaddr * e, socklen_t f) 
{
	return sendto$native(a, b, c, d, (struct sockaddr*)e, f);
}

int	setsockopt(int a, int b, int c, const void * d, socklen_t e)
{
	return setsockopt$native(a, b, c, d, e);
}

int	shutdown(int a, int b)
{
	ns_not_implemented(); /* [XXX][NR!?] */
}

int	sockatmark(int a) 
{
	ns_not_implemented(); /* [XXX][NR!?] */
}

int	socketpair(int a, int b, int c, int *d)
{
	ns_not_implemented(); /* [XXX][NR!?] */
}