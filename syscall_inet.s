/*
 * Core Framework: LibSyscall: syscall_inet.s
 * Copyright (c) 2012 Christina Brooks
 *
 * New native inet syscalls.
 */

#include "NRSyscall.h"

#define InetSyscallEntry(x) .globl x##$native; \
x##$native:

.text
.align 2

InetSyscallEntry(_setsockopt)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #94
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_getsockopt)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #95
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_socket)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #81
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_bind)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #82
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_connect)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #83
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_listen)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #84
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_accept)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #85
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_getsockname)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #86
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_getpeername)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #87
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_socketpair)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #88
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_send)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #89
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_sendto)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #90
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_recv)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #91
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr

InetSyscallEntry(_recvfrom)
stmfd   sp!, {r4, r7}
mov r7, #200
add r7, #92
swi     #0
ldmfd   sp!, {r4, r7}
bx    lr
