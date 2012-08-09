/*
 * Core Framework: LibSyscall: syscall_linux.s
 * Copyright (c) 2012 Christina Brooks
 *
 * System calls that are specific to Linux or/and are 
 * used internally by Core.
 */

#include "NRSyscall.h"

.text
.align 2

SpecialSyscallEntry(_epoll_create)
    stmfd   sp!, {r4, r7}
    mov r7, #200
    add r7, #50
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx    lr

SpecialSyscallEntry(_epoll_ctl)
    stmfd   sp!, {r4, r7}
    mov r7, #200
    add r7, #51
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx    lr

SpecialSyscallEntry(_epoll_wait)
    stmfd   sp!, {r4, r7}
    mov r7, #200
    add r7, #52
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx    lr


L_set_tls_nr:
.long 983045
SpecialSyscallEntry(_set_tls)
    stmfd   sp!, {r4, r7}
    ldr     r7, L_set_tls_nr
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx    lr

SpecialSyscallEntry(_clock_gettime)
    stmfd   sp!, {r4, r7}
    mov r7, #200
    add r7, #63
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx    lr

SpecialSyscallEntry(_gettimeofday)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_gettimeofday
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx    lr

SpecialSyscallEntry(_wait4)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_wait4
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx    lr

SpecialSyscallEntry(_getgid)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_getgid32
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx    lr

SpecialSyscallEntry(_getgroups)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_getgroups32
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx    lr

SpecialSyscallEntry(_getdents64)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_getdents64
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx    lr

SpecialSyscallEntry(_exit_group)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_exit_group
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_ioctl)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_ioctl
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_readv)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_readv
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_writev)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_writev
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_execve)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_execve
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_fork)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_fork
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_exit)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_exit_group
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_futex)
    mov ip, sp
    stmfd sp!, {r4, r5, r6, r7}
    ldmfd ip, {r4, r5, r6}

    mov r7, #200
    add r7, #70
    svc 0
    ldmfd sp!, {r4, r5, r6, r7}
    bx lr

SpecialSyscallEntry(_gettid)
    stmfd   sp!, {r4, r7}
    ldr     r7, =__NR_gettid
    swi     #0
    ldmfd   sp!, {r4, r7}
    bx    lr

SpecialSyscallEntry(_getcwd)
    stmfd sp!, {r4,r7}
    mov r7, #0xB7
    svc 0
    ldmfd sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_pread64)
    stmfd sp!, {r4,r7}
    mov r7, #0xB4
    svc 0
    ldmfd sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_fcntl64)
    stmfd sp!, {r4,r7}
    mov r7, #0xDD
    svc 0
    ldmfd sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_open)
    stmfd sp!, {r4,r7}
    mov r7, #5
    svc 0
    ldmfd sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_lstat)
    stmfd sp!, {r4,r7}
    mov r7, #0xC4
    svc 0
    ldmfd sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_fstat)
    stmfd sp!, {r4,r7}
    mov r7, #197
    svc 0
    ldmfd sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_stat)
    stmfd sp!, {r4,r7}
    mov r7, #195
    svc 0
    ldmfd sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_reboot)
    stmfd sp!, {r4,r7}
    mov r7, #88
    svc 0
    ldmfd sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_brk)
    stmfd sp!, {r4,r7}
    mov r7, #45
    svc 0
    ldmfd sp!, {r4, r7}
    bx lr

SpecialSyscallEntry(_llseek)
    mov r12, sp
    stmfd sp!, {r4-r7}
    ldmia r12, {r4-r6}
    mov r7, #0x8C
    svc 0
    ldmfd sp!, {r4-r7}
    bx lr 

SpecialSyscallEntry(_mmap2)
    mov r12, sp
    stmfd sp!, {r4-r7}
    ldmia r12, {r4-r6}
    mov r7, #192
    svc 0
    ldmfd sp!, {r4-r7}
    bx lr
