/*
 * Core Framework: BSDCompat.h
 * Copyright (c) 2012 Christina Brooks
 *
 * Stuff.
 */

/*
	BSD compatibility stuff. A lot of this stuff is from the 
	Linux compatibility layer from FreeBSD.
 
	See '/sys/compat/linux/' in the fbsd source tree.
 */


/*-
 * Copyright (c) 2000 Assar Westerlund
 * Copyright (c) 1995 Søren Schmidt
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer 
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef _BSD_COMPAT_H_
#define _BSD_COMPAT_H_

/*
 * Provide a separate set of types for the Linux types.
 */
typedef int		l_int;
typedef int32_t		l_long;
typedef int64_t		l_longlong;
typedef short		l_short;
typedef unsigned int	l_uint;
typedef uint32_t	l_ulong;
typedef uint64_t	l_ulonglong;
typedef unsigned short	l_ushort;

typedef char		*l_caddr_t;
typedef l_ulong		l_uintptr_t;
typedef l_long		l_clock_t;
typedef l_int		l_daddr_t;
typedef l_ushort	l_dev_t;
typedef l_uint		l_gid_t;
typedef l_ushort	l_gid16_t;
typedef l_ulong		l_ino_t;
typedef l_int		l_key_t;
typedef l_longlong	l_loff_t;
typedef l_ushort	l_mode_t;
typedef l_long		l_off_t;
typedef l_int		l_pid_t;
typedef l_uint		l_size_t;
typedef l_long		l_suseconds_t;
typedef l_long		l_time_t;
typedef l_uint		l_uid_t;
typedef l_ushort	l_uid16_t;
typedef l_int		l_timer_t;
typedef l_int		l_mqd_t;


/* ******************************************************************** */
#pragma mark Sockets

struct l_sockaddr {
	unsigned short	sa_family;
	char		sa_data[14];
};

struct l_msghdr {
	l_uintptr_t	msg_name;
	l_int		msg_namelen;
	l_uintptr_t	msg_iov;
	l_size_t	msg_iovlen;
	l_uintptr_t	msg_control;
	l_size_t	msg_controllen;
	l_uint		msg_flags;
};

struct l_cmsghdr {
	l_size_t	cmsg_len;
	l_int		cmsg_level;
	l_int		cmsg_type;
};

struct l_ifmap {
	l_ulong		mem_start;
	l_ulong		mem_end;
	l_ushort	base_addr;
	u_char		irq;
	u_char		dma;
	u_char		port;
};

/* msg flags in recvfrom/recvmsg */

#define LINUX_MSG_OOB		0x01
#define LINUX_MSG_PEEK		0x02
#define LINUX_MSG_DONTROUTE	0x04
#define LINUX_MSG_CTRUNC	0x08
#define LINUX_MSG_PROXY		0x10
#define LINUX_MSG_TRUNC		0x20
#define LINUX_MSG_DONTWAIT	0x40
#define LINUX_MSG_EOR		0x80
#define LINUX_MSG_WAITALL	0x100
#define LINUX_MSG_FIN		0x200
#define LINUX_MSG_SYN		0x400
#define LINUX_MSG_CONFIRM	0x800
#define LINUX_MSG_RST		0x1000
#define LINUX_MSG_ERRQUEUE	0x2000
#define LINUX_MSG_NOSIGNAL	0x4000
#define LINUX_MSG_CMSG_CLOEXEC	0x40000000

/* Socket-level control message types */

#define LINUX_SCM_RIGHTS	0x01
#define LINUX_SCM_CREDENTIALS   0x02

/* Ancilliary data object information macros */

/* Supported address families */

#define	LINUX_AF_UNSPEC		0
#define	LINUX_AF_UNIX		1
#define	LINUX_AF_INET		2
#define	LINUX_AF_AX25		3
#define	LINUX_AF_IPX		4
#define	LINUX_AF_APPLETALK	5
#define	LINUX_AF_INET6		10

/* Supported socket types */

#define	LINUX_SOCK_STREAM	1
#define	LINUX_SOCK_DGRAM	2
#define	LINUX_SOCK_RAW		3
#define	LINUX_SOCK_RDM		4
#define	LINUX_SOCK_SEQPACKET	5

#define	LINUX_SOCK_MAX		LINUX_SOCK_SEQPACKET

#define	LINUX_SOCK_TYPE_MASK	0xf

/* Flags for socket, socketpair, accept4 */

#define	LINUX_SOCK_CLOEXEC	LINUX_O_CLOEXEC
#define	LINUX_SOCK_NONBLOCK	LINUX_O_NONBLOCK

/*
 * Socket defines
 */
#define	LINUX_SOCKET 		1
#define	LINUX_BIND		2
#define	LINUX_CONNECT 		3
#define	LINUX_LISTEN 		4
#define	LINUX_ACCEPT 		5
#define	LINUX_GETSOCKNAME	6
#define	LINUX_GETPEERNAME	7
#define	LINUX_SOCKETPAIR	8
#define	LINUX_SEND		9
#define	LINUX_RECV		10
#define	LINUX_SENDTO 		11
#define	LINUX_RECVFROM 		12
#define	LINUX_SHUTDOWN 		13
#define	LINUX_SETSOCKOPT	14
#define	LINUX_GETSOCKOPT	15
#define	LINUX_SENDMSG		16
#define	LINUX_RECVMSG		17
#define	LINUX_ACCEPT4		18

#define	LINUX_SOL_SOCKET	1
#define	LINUX_SOL_IP		0
#define	LINUX_SOL_IPX		256
#define	LINUX_SOL_AX25		257
#define	LINUX_SOL_TCP		6
#define	LINUX_SOL_UDP		17

#define	LINUX_SO_DEBUG		1
#define	LINUX_SO_REUSEADDR	2
#define	LINUX_SO_TYPE		3
#define	LINUX_SO_ERROR		4
#define	LINUX_SO_DONTROUTE	5
#define	LINUX_SO_BROADCAST	6
#define	LINUX_SO_SNDBUF		7
#define	LINUX_SO_RCVBUF		8
#define	LINUX_SO_KEEPALIVE	9
#define	LINUX_SO_OOBINLINE	10
#define	LINUX_SO_NO_CHECK	11
#define	LINUX_SO_PRIORITY	12
#define	LINUX_SO_LINGER		13
#define	LINUX_SO_PEERCRED	17
#define	LINUX_SO_RCVLOWAT	18
#define	LINUX_SO_SNDLOWAT	19
#define	LINUX_SO_RCVTIMEO	20
#define	LINUX_SO_SNDTIMEO	21
#define	LINUX_SO_TIMESTAMP	29
#define	LINUX_SO_ACCEPTCONN	30

#define	LINUX_IP_TOS		1
#define	LINUX_IP_TTL		2
#define	LINUX_IP_HDRINCL	3
#define	LINUX_IP_OPTIONS	4

#define	LINUX_IP_MULTICAST_IF		32
#define	LINUX_IP_MULTICAST_TTL		33
#define	LINUX_IP_MULTICAST_LOOP		34
#define	LINUX_IP_ADD_MEMBERSHIP		35
#define	LINUX_IP_DROP_MEMBERSHIP	36

#endif /* _LINUX_SOCKET_H_ */