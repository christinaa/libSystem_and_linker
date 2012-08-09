/*
 * core: mach_thread.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Something.
 *
 * This is a part of MachThreads. It should not be complied along with
 * OSThread.
 */

#include "pthread_internals.h"
#include "pthread_workqueue.h"

#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <sys/queue.h>
#include <machine/vmparam.h>
#include <mach/vm_statistics.h>
#include <mach/mach.h>
#include <signal.h>

#include "OSLog.h"

extern int set_tls$LINUX(void *ptr);

/* Get the TLS pointer set by the kernel thing or something
   mrc	p15, 0, r0, c13, c0, 3
 */
#define __get_tls() \
({ register unsigned int __val asm("r0"); \
asm ("mrc p15, 0, r0, c13, c0, 3" : "=r"(__val) ); \
(volatile void*)__val; })

#define not_implemented() OSHalt("%s: not implemented", __FUNCTION__)

int ______NumCPUs = 1;
mach_port_t thread_recycle_port;

int _NumCPUs(void) {
	return ______NumCPUs;
}

void _pthread_tsd_cleanup4(pthread_t self)
{
	not_implemented();
}

int __pthread_sigmask(int how, const sigset_t *restrict set,
					sigset_t *restrict oset)
{
	not_implemented();
}

int __semwait_signal(int cond_sem, int mutex_sem, int timeout, int relative, __int64_t tv_sec, __int32_t tv_nsec)
{
	not_implemented();
}

void set_malloc_singlethreaded(int is)
{
	OSLog("set_malloc_singlethreaded(%d)", is);
}

int __sigwait(const sigset_t *set, int *sig)
{
	not_implemented();
}

void __pthread_set_self(pthread_t* pt)
{
	set_tls$LINUX((void*)pt);
}

int
sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	OSLog("sysctl(%s): not implemented!", name);
	
	return 0;
}

int
sysctlbyname(const char *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	not_implemented();
}

extern int pthread_init(void);

void _OSThreadInitialize(void)
{
	/* oh, boy, here we go ... */
	pthread_init();
}

kern_return_t thread_set_state
(
 thread_act_t target_act,
 thread_state_flavor_t flavor,
 thread_state_t new_state,
 mach_msg_type_number_t new_stateCnt
)
{
	not_implemented();
}

kern_return_t vm_map
(
 vm_map_t target_task,
 vm_address_t *address,
 vm_size_t size,
 vm_address_t mask,
 int flags,
 mem_entry_name_port_t object,
 vm_offset_t offset,
 boolean_t copy,
 vm_prot_t cur_protection,
 vm_prot_t max_protection,
 vm_inherit_t inheritance
 )
{
	not_implemented();
}

kern_return_t thread_terminate(thread_act_t target_thread)
{
	not_implemented();
}

kern_return_t   vm_protect
(mach_port_t           target_task,
 vm_address_t            address,
 vm_size_t                  size,
 boolean_t           set_maximum,
 vm_prot_t        new_protection)
{
	not_implemented();
}

kern_return_t thread_create_running
(
 task_t parent_task,
 thread_state_flavor_t flavor,
 thread_state_t new_state,
 mach_msg_type_number_t new_stateCnt,
 thread_act_t *child_act
 ) 
{
	not_implemented();
}

kern_return_t thread_create(task_t parent_task, thread_act_t* child_thread)
{
	not_implemented();
}

kern_return_t   mach_port_type
(ipc_space_t                               task,
 mach_port_name_t                          name,
 mach_port_type_t*                         ptype)
{
	not_implemented();
}

void start_wqthread(pthread_t self, mach_port_t kport, void * stackaddr, pthread_workitem_t item, int reuse)
{
	not_implemented();
}

void dispatch_atfork_prepare(void) {
	not_implemented();
}
void dispatch_atfork_parent(void) {
	not_implemented();
}
void dispatch_atfork_child(void) {
	not_implemented();
}

extern boolean_t swtch_pri(int pri)
{
	not_implemented();
}

extern boolean_t swtch(void)
{
	not_implemented();
}

void _init_cpu_capabilities(void)
{
	OSLog("_init_cpu_capabilities(): no idea!");
}

#define WORKQUEUE_OVERCOMMIT 0x10000

extern int __workq_kernreturn(int x, pthread_workitem_t wq, int x1, int x2)
{
	not_implemented();
}