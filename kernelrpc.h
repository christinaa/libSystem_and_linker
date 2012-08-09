/*
 * Core Framework: LibSyscall: kernelrpc.h
 * Copyright (c) 2012 Christina Brooks
 *
 * Kernel RPC header.
 */

#ifndef core_kernelrpc_h
#define core_kernelrpc_h

#include <stdio.h>
#include <mach/mach.h>
#include <sys/mman.h>
#include "OSLog.h"

#define kernel_rpc(x) _kernelrpc_##x##_trap



#pragma mark Structures
/********************************************************/

typedef struct {
	mach_msg_header_t* msg;
	mach_msg_option_t option;
	mach_msg_size_t send_size;
	mach_msg_size_t receive_limit;
	mach_port_t receive_name;
	mach_msg_timeout_t timeout;
	mach_port_t notify;
} mach_msg_trap_data_t;




#pragma mark External Symbols
/********************************************************/

/* gah, kernel and user defs are different so using u32 in place of mach_vm_address */
extern kern_return_t mach_msg_overwrite_trap(uint32_t msg,
											 mach_msg_option_t option,
											 mach_msg_size_t send_size,
											 mach_msg_size_t rcv_size,
											 mach_port_name_t rcv_name,
											 mach_msg_timeout_t timeout,
											 mach_port_name_t notify,
											 uint32_t rcv_msg);

extern kern_return_t kernel_rpc(vm_allocate)
(vm_map_t target_task, vm_address_t *address, vm_size_t size, int flags);

extern kern_return_t kernel_rpc(mach_msg)
(mach_msg_trap_data_t* data);

extern mach_port_t kernel_rpc(task_self)
(void);

extern kern_return_t kernel_rpc(task_for_pid)
(mach_port_name_t target_tport, int pid, mach_port_name_t *t);

extern kern_return_t kernel_rpc(pid_for_task)
(mach_port_name_t target_tport, int* pid);

extern kern_return_t kernel_rpc(mach_port_allocate)
(ipc_space_t task, mach_port_right_t right, mach_port_name_t *name);

extern kern_return_t kernel_rpc(mach_port_deallocate)
(ipc_space_t task, mach_port_name_t name);

extern kern_return_t kernel_rpc(mach_port_destroy)
(ipc_space_t task, mach_port_name_t name);

extern kern_return_t kernel_rpc(mach_port_insert_right)
(ipc_space_t task, mach_port_name_t name, mach_port_t poly, mach_msg_type_name_t polyPoly);

extern kern_return_t kernel_rpc(mach_port_mod_refs)
(ipc_space_t task, mach_port_name_t name, mach_port_right_t right, mach_port_delta_t delta);

extern kern_return_t kernel_rpc(mach_port_insert_member)
(ipc_space_t task, mach_port_name_t name, mach_port_name_t pset);

#endif
