/*
 * Core Framework: PT: LX_sched
 * Copyright (c) 2012 Christina Brooks
 *
 * Constants for the Linux scheduler.
 */

#ifndef core_LX_sched_h
#define core_LX_sched_h

#define LX_CSIGNAL         0x000000ff      /* signal mask to be sent at exit */
#define LX_CLONE_VM        0x00000100      /* set if VM shared between processes */
#define LX_CLONE_FS        0x00000200      /* set if fs info shared between processes */
#define LX_CLONE_FILES     0x00000400      /* set if open files shared between processes */
#define LX_CLONE_SIGHAND   0x00000800      /* set if signal handlers and blocked signals shared */
#define LX_CLONE_PTRACE    0x00002000      /* set if we want to let tracing continue on the child too */
#define LX_CLONE_VFORK     0x00004000      /* set if the parent wants the child to wake it up on mm_release */
#define LX_CLONE_PARENT    0x00008000      /* set if we want to have the same parent as the cloner */
#define LX_CLONE_THREAD    0x00010000      /* Same thread group? */
#define LX_CLONE_NEWNS     0x00020000      /* New namespace group? */
#define LX_CLONE_SYSVSEM   0x00040000      /* share system V SEM_UNDO semantics */
#define LX_CLONE_SETTLS    0x00080000      /* create a new TLS for the child */
#define LX_CLONE_PARENT_SETTID     0x00100000      /* set the TID in the parent */
#define LX_CLONE_CHILD_CLEARTID    0x00200000      /* clear the TID in the child */
#define LX_CLONE_DETACHED          0x00400000      /* Unused, ignored */
#define LX_CLONE_UNTRACED          0x00800000      /* set if the tracing process can't force CLONE_PTRACE on this clone */
#define LX_CLONE_CHILD_SETTID      0x01000000      /* set the TID in the child */
/* 0x02000000 was previously the unused CLONE_STOPPED (Start in stopped state)
    and is now available for re-use. */
#define LX_CLONE_NEWUTS            0x04000000      /* New utsname group? */
#define LX_CLONE_NEWIPC            0x08000000      /* New ipcs */
#define LX_CLONE_NEWUSER           0x10000000      /* New user namespace */
#define LX_CLONE_NEWPID            0x20000000      /* New pid namespace */
#define LX_CLONE_NEWNET            0x40000000      /* New network namespace */
#define LX_CLONE_IO                0x80000000      /* Clone io context */

#endif
