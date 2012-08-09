/*
 * Core Framework: LibSyscall: syscall_mach.s
 * Copyright (c) 2012 Christina Brooks
 *
 * Mach system calls.
 *    > All mach syscalls are masked with 0xffffff00.
 *    > The 'svc 0x80' interface must be used.
 *    > Mach kernel IPC trap naming conventions (_kernelrpc_<name>_trap).
 *    > BSD syscalls use normal naming conventions.
 */


.text
.align 2

#define declare_fast_trap(x, y) \
.globl _##x; \
.globl __kernelrpc_##x##_trap; \
__kernelrpc_##x##_trap: \
_##x: \
mov r12, y; \
svc 0x80; \
bx lr;

declare_fast_trap(mach_thread_self, #-27);
declare_fast_trap(mach_host_self, #-29);

declare_fast_trap(host_info, #-108);
declare_fast_trap(thread_policy, #-109);

declare_fast_trap(__bsdthread_register_trap, #-110);
declare_fast_trap(__thread_selfid_trap, #-111);
declare_fast_trap(__bsdthread_create_trap, #-112);

declare_fast_trap(semaphore_signal, #-33)
declare_fast_trap(semaphore_signal_all, #-34)
declare_fast_trap(semaphore_signal_thread, #-35)
declare_fast_trap(semaphore_wait, #-36)
declare_fast_trap(semaphore_wait_signal, #-37)
declare_fast_trap(semaphore_timedwait, #-38)
declare_fast_trap(semaphore_timedwait_signal, #-39)

declare_fast_trap(semaphore_create, #-113)

declare_fast_trap(__disable_threadsignal, #-115);
declare_fast_trap(syscall_thread_switch, #-116);
declare_fast_trap(__bsdthread_terminate, #-117);
declare_fast_trap(__pthread_canceled, #-118);
declare_fast_trap(__pthread_kill, #-119);
declare_fast_trap(__pthread_markcancel, #-120);
declare_fast_trap(__workq_open, #-121);

declare_fast_trap(thread_switch, #-122);
declare_fast_trap(task_threads, #-123);
declare_fast_trap(thread_get_state, #-124);

declare_fast_trap(thread_suspend, #-125);
declare_fast_trap(thread_resume, #-126);

/*******************************************************/

#define bsd_trap(x) .globl _##x; \
_##x

#define mach_trap(x) .globl _##x; \
_##x

bsd_trap(__getdirentries64):
	mov r12, #0x158
	svc 0x80
	bx lr

bsd_trap(lseek_lol):
	mov r12, #199
	svc 0x80
	bx lr

/*******************************************************/
/*
 mach_port_name_t mk_timer_create(void)
 kern_return_t mk_timer_destroy(mach_port_name_t name)
 kern_return_t mk_timer_arm(mach_port_name_t name, uint64_t expire_time)
 kern_return_t mk_timer_cancel(mach_port_name_t name, uint64_t *result_time)
 */
mach_trap(mk_timer_create):
	mov r12, #0xFFFFFFA5
	svc 0x80
	bx lr

mach_trap(mk_timer_arm):
	mov r12, #0xFFFFFFA3
	svc 0x80
	bx lr

mach_trap(mk_timer_cancel):
	mov r12, #0xFFFFFFA2
	svc 0x80
	bx lr

mach_trap(mk_timer_destroy):
	mov r12, #0xFFFFFFA4
	svc 0x80
	bx lr

/*******************************************************/

/*
 * Here, we got 8 arguments to the syscall.
 * They first four go in the usual places (r0-r3) while
 * the other ones go on r4, r5, r6 and r8.
 */
mach_trap(mach_msg_overwrite_trap):
	mov r12, sp
	push {r4-r6, r8}
	
	ldr r4, [r12]
	ldr r5, [r12, #4]
	ldr r6, [r12, #8]
	ldr r8, [r12, #12]

	mov r12, #0xffffffe0
	svc 0x80

	pop {r4-r6, r8}
	bx lr



/*******************************************************/

#define kernel_rpc(x) .globl __kernelrpc_##x##_trap; \
__kernelrpc_##x##_trap

kernel_rpc(mach_port_insert_member):
	mov r12, #0xffffffea
	svc 0x80
	bx lr

kernel_rpc(mach_port_mod_refs):
	mov r12, #0xffffffed
	svc 0x80
	bx lr

kernel_rpc(mach_port_destroy):
	mov r12, #0xffffffef
	svc 0x80
	bx lr

kernel_rpc(mach_port_deallocate):
	mov r12, #0xffffffee
	svc 0x80
	bx lr

kernel_rpc(mach_port_insert_right):
	mov r12, #0xffffffeb
	svc 0x80
	bx lr

kernel_rpc(mach_port_allocate):
	mov r12, #0xfffffff0
	svc 0x80
	bx lr

kernel_rpc(mach_msg):
	mov r12, #0xffffffe1
	svc 0x80
	bx lr

kernel_rpc(vm_allocate):
	mov r12, #0xfffffff5
	svc 0x80
	bx lr

kernel_rpc(task_self):
	mov r12, #0xffffffe4
	svc 0x80
	bx lr

kernel_rpc(task_for_pid):
	mov r12, #0xffffffd3
	svc 0x80
	bx lr

kernel_rpc(pid_for_task):
	mov r12, #0xffffffd2
	svc 0x80
	bx lr

