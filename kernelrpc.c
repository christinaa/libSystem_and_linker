/*
 * Core Framework: LibSyscall: kernelrpc.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Kernel RPC traps.
 *
 * Technically, these could be called directly, but 
 * apparently it is better to add small stubs for the
 * mach traps (the same way as libsystem_kernel does).
 *
 * The actual traps live in 'syscall_mach.s'.
 */


#include "kernelrpc.h"

#define krpc_log(x, ...) OSLogLib("kernel_rpc", x, ##__VA_ARGS__)
#define do_if_debug() if (1)

typedef struct {
	void* a;
	void* b;
	int c;
	void* d;
	void* e;
	uint64_t f;
} bsd_reg_args_t;

typedef struct {
	void* func;
	void* func_arg;
	void* stack;
	void* thread;
	unsigned int flags;
} bsd_crt_args_t;

/* XXX: this needs to be moved */
extern void _ZN3lnk4ldbg11printNumberEPKci(const char* ch, int num);
long lseek_trap(unsigned int fd, long long off, int whence);
long long lseek(unsigned int fd, long long off, int whence)
{
	long ret = lseek_trap(fd, (long)off, whence);
	return (long long)ret;
}

extern int __bsdthread_register_trap(bsd_reg_args_t* args);
extern void* __bsdthread_create_trap(bsd_crt_args_t* args);
extern int __thread_selfid_trap(uint64_t* ret);

uint64_t
__thread_selfid()
{
	uint64_t ret;
	__thread_selfid_trap(&ret);
	return ret; 
}

int
__bsdthread_register(void* a, void* b, int c, void* d, void* e, uint64_t f)
{
	bsd_reg_args_t args;
	
	args.a = a;
	args.b = b;
	args.c = c;
	args.d = d;
	args.e = e;
	args.f = f;
	
	return __bsdthread_register_trap(&args);
}

void* __bsdthread_create(void *(*func)(void *), void * func_arg, void * stack, void* thread, unsigned int flags)
{
	bsd_crt_args_t args;
	
	args.func = (void*)func;
	args.func_arg = func_arg;
	args.stack = stack;
	args.thread = thread;
	args.flags = flags;
	
	return __bsdthread_create_trap(&args);
}

#pragma mark Ports
/********************************************************/

kern_return_t
mach_port_allocate(ipc_space_t task, mach_port_right_t right, mach_port_name_t *name)
{
	return kernel_rpc(mach_port_allocate)(task, right, name);
}

kern_return_t
mach_port_insert_member(ipc_space_t task, mach_port_name_t name, mach_port_name_t pset)
{
	return kernel_rpc(mach_port_insert_member)(task, name, pset);
}

kern_return_t mach_port_deallocate(ipc_space_t task, mach_port_name_t name)
{

	return kernel_rpc(mach_port_deallocate)(task, name);
}

kern_return_t mach_port_destroy(ipc_space_t task, mach_port_name_t name)
{
	return kernel_rpc(mach_port_destroy)(task, name);
}

kern_return_t mach_port_insert_right(ipc_space_t task, mach_port_name_t name, mach_port_t poly, mach_msg_type_name_t polyPoly)
{
	return kernel_rpc(mach_port_insert_right)(task, name, poly, polyPoly);
}

kern_return_t mach_port_mod_refs(ipc_space_t task,
								 mach_port_name_t name,
								 mach_port_right_t right,
								 mach_port_delta_t delta)
{
	return kernel_rpc(mach_port_mod_refs)(task, name, right, delta);
}




#pragma mark VM API
/********************************************************/

/* vm_allocate: Similar to mmap. Allocates vm. */
kern_return_t vm_allocate(vm_map_t target_task, vm_address_t *address, vm_size_t size, int flags)
{
	return kernel_rpc(vm_allocate)(target_task, address, size, flags);
}




#pragma mark Tasks
/********************************************************/

mach_port_t task_self(void)
{
	return kernel_rpc(task_self)();
}

kern_return_t task_for_pid(mach_port_name_t target_tport, int pid, mach_port_name_t *t)
{
	return kernel_rpc(task_for_pid)(target_tport, pid, t);
}

kern_return_t pid_for_task(mach_port_name_t t, int *x)
{
	return kernel_rpc(pid_for_task)(t, x);
}




#pragma mark Mach Message Traps
/********************************************************/

extern mach_msg_return_t mach_msg(mach_msg_header_t *msg,
								  mach_msg_option_t option,
								  mach_msg_size_t send_size,
								  mach_msg_size_t rcv_size,
								  mach_port_name_t rcv_name,
								  mach_msg_timeout_t timeout,
								  mach_port_name_t notify)
{
	krpc_log("mach_msg_trap {msg=%p, option=%d, loc=%d, rem=%d}", msg, option, msg->msgh_local_port, msg->msgh_remote_port);
	
	/* trap */
	mach_msg_return_t ret;
	
	ret = mach_msg_overwrite_trap((uint32_t)msg, option, send_size, rcv_size, rcv_name, timeout, notify, 0);
	
	do_if_debug() {
		if (ret != KERN_SUCCESS) {
			krpc_log("mach_msg_trap(%p) = %d", msg, ret);
		}
	}
	
	return ret;
}