/*
 * Core Framework: OSThread.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Threading.
 *
 * NOTE: THIS WILL NOT WORK ON NON-ARMv7 MACHINES!
 */

/*
 * The core logic is here. All the getters setters for weird
 * bits are not implemented. They're easy to do. But I'm lazy.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "OSLog.h"
#include <errno.h>
#include <libkern/OSAtomic.h>
#include "OSThread.h"



/**********************************************************************************/
#pragma mark Definitions

/* Clone constants */
#include "LX_sched.h"

/* System calls */
extern int clone$LINUX(int (*fn)(void*, void *), void *child_stack, int flags, void*  arg, ...);
extern int futex$LINUX(int *uaddr, int op, int val, const struct timespec *timeout, int *uaddr2, int val3);
extern int set_tls$LINUX(void *ptr);

/* Get the TLS pointer set by the kernel thing or something */
#define OSGetCurrentThread() \
({ register unsigned int __val asm("r0"); \
	asm ("mrc p15, 0, r0, c13, c0, 3" : "=r"(__val) ); \
	(volatile void*)__val; })

/* Futex constants */
#define FUTEX_WAIT  0
#define FUTEX_WAKE  1
#define FUTEX_FD    2
#define FUTEX_REQUEUE     3
#define FUTEX_CMP_REQUEUE 4

/* Signals */
#define LX_SIGCHLD 17

/* Conversions */
#define GetOSMutex(x) ((OSMutex*)x)
#define GetOSMutexAttributes(x) ((OSMutexAttributes*)x)
#define GetOSThreadAttributes(x) ((OSThreadAttributes*)x)
#define GetOSThread(x) _getOSThread(x)
#define GetTlsMap(x) (OSTlsEntry*)((uintptr_t)x + sizeof(OSThread))

/* Logging */
#define OSThreadLog(x, ...) OSLogLib("OSThread", x, ##__VA_ARGS__)
#define OSThreadNotImplemented() OSHalt("OSThread: (%s:%d) not implemented ", __FUNCTION__, __LINE__)

/* Very verbose logging */
static int gThreadLog = 0;

/* something something */
/*
 typedef union {
 struct {
 long __sig;
 struct __darwin_pthread_handler_rec  *__cleanup_stack; 
 
 OSThread thread;
 } data;
 char pad[__PTHREAD_SIZE__];
 } uni_OSThread;
 */

typedef struct {
	/*
	 * Non-opaque bits.
	 */
	long sig;
	struct __darwin_pthread_handler_rec  *cleanup_stack; 
	
	/*
	 * Top of the thread's stack
	 */
	void* top_stack;
	
	/*
	 * Entry point and the arg passed to the thread.
	 */
	void* (*thread_entry)(void *);
	void* thread_arg; 
	
	/*
	 * Thread's process identifier.
	 */
	pid_t pid;
	
	/*
	 * Thread attributes.
	 */
	pthread_attr_t attr;
	
	/*
	 * Mach thread.
	 */
	mach_port_t kernel_thread;
} OSThread;

typedef struct {
	long sig;
	OSLowLock control;
} OSThreadOnce;

typedef unsigned long OSThreadKey;

typedef struct {
	/* Darwin */
	long sig;
	
	/* Stuff */
	uint32_t type;
	int32_t count;
	pid_t owner;
	
	uint32_t initialized;
	OSLowLock lll;
} OSMutex;

typedef struct {
	OSThreadKey key;
	void* val;
	void* destr;
	int allocated;
} OSTlsEntry;

typedef struct {
	long sig;
	uint32_t type;
} OSMutexAttributes;

typedef struct {
	/* Darwin */
	long sig;
	
	/* Us */
	size_t stack_size;
	int detach_state;
} OSThreadAttributes;

/*
	Configuration for the threading library.
 */
#define OS_DEFAULT_THREAD_STACK 1024*64
#define OS_TLS_COUNT 100
#define OS_TLS_SIZE OS_TLS_COUNT*sizeof(OSTlsEntry)
#define OS_TLS_UK_START 10 /* user key start */

/*
	TLS area for the main thread.
	This is like all the other ones, except it doesn't have a stack as the
	kernel sets up the stack for the main thread.
 */
char _mainThreadTLS[sizeof(OSThread) + OS_TLS_SIZE] = {0};

/* Global lock for accessing TLS lists */
OSLowLock tls_lock;

/* Global lock for recursive mutex operations */
OSLowLock recursive_op_lock;

/* IsThreaded flag */
int __is_threaded = 1;


/**********************************************************************************/
#pragma mark Initializer

/*
	Initialization code for the threading library.
 */
void _OSThreadInitialize(void)
{
	/* validate if the ABI is compat */
	if ((sizeof(long) + __PTHREAD_ONCE_SIZE__) < sizeof(OSThreadOnce)) {
		OSHalt("Inconsistent ABI: OSThreadOnce too big");
	}
	if ((sizeof(long) + sizeof(struct __darwin_pthread_handler_rec*) + __PTHREAD_SIZE__) < sizeof(OSThread)) {
		OSHalt("Inconsistent ABI: OSThread too big");
	}
	if ((sizeof(long) + __PTHREAD_MUTEX_SIZE__) < sizeof(OSMutex)) {
		OSHalt("Inconsistent ABI: OSMutex too big");
	}
	if ((sizeof(long) + __PTHREAD_ATTR_SIZE__) < sizeof(OSThreadAttributes)) {
		OSHalt("Inconsistent ABI: OSThreadAttributes too big");
	}
	if ((sizeof(long) + __PTHREAD_MUTEXATTR_SIZE__) < sizeof(OSMutexAttributes)) {
		OSHalt("Inconsistent ABI: OSMutexAttributes too big");
	}
	
	/* setup main TLS */
	set_tls$LINUX(&_mainThreadTLS);
	if (OSGetCurrentThread() == 0) {
		OSHalt("OSThread: failed to set TLS bit for the main thread");
	}
	
	/* fill out the main thread info */
	OSThread* mainThread = (OSThread*)OSGetCurrentThread();
	mainThread->pid = 0;
	
	OSThreadLog("init: done {m=%p, tls_c=%d, tls_s=%d, s=%d}",
				OSGetCurrentThread(),
				OS_TLS_COUNT,
				OS_TLS_SIZE,
				OS_DEFAULT_THREAD_STACK);
}




/**********************************************************************************/
#pragma mark Internal API

static OSThread* _getOSThread(pthread_t pt)
{
	if (pt) {
		return (OSThread*)pt;
	}
	else {
		/* pt==0 is a synonym for "main thread" that always works */
		return (OSThread*)(&_mainThreadTLS);
	}
}



/**********************************************************************************/
#pragma mark Thread Conditions

int pthread_cond_init(pthread_cond_t * __restrict cond,
					  const pthread_condattr_t * __restrict attr)
{
	OSThreadNotImplemented();
}

int pthread_cond_signal(pthread_cond_t * cond)
{
	OSThreadNotImplemented();
}

int pthread_cond_timedwait(pthread_cond_t * __restrict cond,
						   pthread_mutex_t * __restrict mutex,
						   const struct timespec * __restrict ts)
{
	OSThreadNotImplemented();
}

int	pthread_cond_wait(pthread_cond_t * __restrict cond,
					  pthread_mutex_t * __restrict mutex)
{
	OSThreadLog("pthread_cond_wait(%p, %p)", cond, mutex);
	return 0;
}

int pthread_cond_broadcast(pthread_cond_t *c)
{
	//OSThreadLog("pthread_cond_broadcast(%p)");
	return 0;
}




/**********************************************************************************/
#pragma mark Thread Loacal Storage

/*
	Thread local storage implementation.
 
	TLS bits are stored just after the OSThread header in the thread's
	space bit. The OS_TLS_SIZE macro specifies the allowed size of TLS entries
	while the OS_TLS_COUNT macro specifies the maximum count of the TLS entries.
 */

void pthread_key_init_np(pthread_key_t key, void (*dest)(void *)) 
{
	OSThreadLog("pthread_key_init_np(%d, %p)", key, dest);
	
	if (OSGetCurrentThread() == 0) {
		OSHalt("pthread_key_init_np: Current thread invalid");
	}
	
	lll_lock(&tls_lock);
	
	/* get TLS list */
	OSTlsEntry* list = GetTlsMap(OSGetCurrentThread());
	OSTlsEntry* entry = NULL;
	OSThreadKey rk = 0;
	
	for (int i = 0; i < OS_TLS_COUNT; i++) {
		if (key == i) {
			entry = &list[i];
			rk = i;
			break;
		}
	}
	
	if (entry != NULL) {
		entry->key = rk;
		entry->destr = dest;
		entry->val = NULL;
		entry->allocated = 1;
	}
	else {
		OSHalt("pthread_key_init_np: bad key id");
	}
	
	lll_unlock(&tls_lock);
	
	if (gThreadLog) {
		OSThreadLog("pthread_key_init_np: Force-inited thread key {i=%d}", rk);
	}
}

static uint32_t thing = 0;

void *pthread_getspecific(pthread_key_t key)
{	
	lll_lock(&tls_lock);
	
	/* get TLS list */
	OSTlsEntry* list = GetTlsMap(OSGetCurrentThread());
	OSTlsEntry* entry = NULL;
	void* ret = NULL;
	
	for (int i = 0; i < OS_TLS_COUNT; i++) {
		if (list[i].key == key) {
			entry = &list[i];
		}
	}
	
	if (entry != NULL) {
		ret = entry->val;
	}

	lll_unlock(&tls_lock);
	
	if (gThreadLog) {
		OSThreadLog("GETspecific: %d (value: %p, t: %d) ", key, ret, thing);
	}
	
	/*
	if (thing > 10000) {
		OSLog("TRAP TIME!");
		int* gg = NULL;
		*gg = 1234;
	}
	thing++;
	*/
	
	return ret;
}

int pthread_key_create(pthread_key_t *key, void (*destr)(void *))
{
	if (OSGetCurrentThread() == 0) {
		OSHalt("pthread_key_create: Current thread invalid");
	}
	
	lll_lock(&tls_lock);
	
	/* get TLS list */
	OSTlsEntry* list = GetTlsMap(OSGetCurrentThread());
	OSTlsEntry* entry = NULL;
	OSThreadKey rk = 0;
	
	/* skip the reserved keys */
	for (int i = OS_TLS_UK_START; i < OS_TLS_COUNT; i++) {
		if (!list[i].allocated) {
			entry = &list[i];
			rk = i;
			break;
		}
	}
	
	if (entry != NULL) {
		entry->key = rk;
		entry->destr = destr;
		entry->val = NULL;
		entry->allocated = 1;
	}
	else {
		OSHalt("pthread_key_create: Out of memory in the TLS area.");
	}
	
	lll_unlock(&tls_lock);
	
	if (gThreadLog) {
		 OSThreadLog("Created thread key {i=%d}", rk);
	}
	
	*key = rk;
	
	return 0;
}

int pthread_setspecific(pthread_key_t key, const void *ptr)
{
	if (gThreadLog) {
		 OSThreadLog("SETspecific: %d, %p", key, ptr);
	}
	
	lll_lock(&tls_lock);
	
	/* get TLS list */
	OSTlsEntry* list = GetTlsMap(OSGetCurrentThread());
	OSTlsEntry* entry = NULL;
	
	for (int i = 0; i < OS_TLS_COUNT; i++) {
		if (list[i].key == key) {
			entry = &list[i];
		}
	}
	
	if (entry != NULL) {
		entry->val = (void*)ptr;
	}
	
	lll_unlock(&tls_lock);
	
	return 0;
}

int pthread_key_delete(pthread_key_t key)
{
	OSThreadNotImplemented();
}

int pthread_join(pthread_t thread, void ** vd)
{
	OSThreadNotImplemented();
}




/**********************************************************************************/
#pragma mark Thread Attributes 

int pthread_attr_destroy(pthread_attr_t* attr) 
{
	/*
		Well, we didn't allocate it so why do anything?
	 */
	return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t* attr, int* rs)
{
	OSThreadNotImplemented();
}

int pthread_attr_getguardsize(const pthread_attr_t * attr, size_t * rs)
{
	OSThreadNotImplemented();
}

int pthread_attr_getinheritsched(const pthread_attr_t * attr, int * rs)
{
	OSThreadNotImplemented();
}

int pthread_attr_getschedparam(const pthread_attr_t * attr, struct sched_param * rs)
{
	OSThreadNotImplemented();
}

int pthread_attr_getschedpolicy(const pthread_attr_t * attr, int * rs)
{
	OSThreadNotImplemented();
}

int pthread_attr_getscope(const pthread_attr_t * attr, int * rs)
{
	OSThreadNotImplemented();
}

int pthread_attr_getstack(const pthread_attr_t * attr, void ** rs, size_t * sz)
{
	OSThreadNotImplemented();
}

int pthread_attr_getstackaddr(const pthread_attr_t * attr, void ** rs)
{
	OSThreadNotImplemented();
}

int pthread_attr_getstacksize(const pthread_attr_t * attr, size_t * rs)
{
	OSThreadNotImplemented();
}

int pthread_attr_init(pthread_attr_t *attr)
{
	OSThreadAttributes* tt = GetOSThreadAttributes(attr);
	
	tt->sig = 0;
	tt->stack_size = OS_DEFAULT_THREAD_STACK;
	tt->detach_state = 0;
	
	return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t * attr, int rs)
{
	OSThreadAttributes* tt = GetOSThreadAttributes(attr);
	tt->detach_state = rs;
	return 0;
}

int pthread_attr_setguardsize(pthread_attr_t * attr, size_t rs)
{
	OSThreadNotImplemented();
}

int pthread_attr_setinheritsched(pthread_attr_t *attr, int rs)
{
	OSThreadNotImplemented();
}

int pthread_attr_setschedparam(pthread_attr_t * attr, const struct sched_param * rs)
{
	OSThreadNotImplemented();
}

int pthread_attr_setstacksize(pthread_attr_t * attr, size_t rs)
{
	OSThreadAttributes* tt = GetOSThreadAttributes(attr);
	tt->stack_size = rs;
	return 0;
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int rs)
{
	OSThreadNotImplemented();
}

int pthread_attr_setscope(pthread_attr_t * attr, int rs)
{
	//OSThreadNotImplemented();
	return 0;
}

int pthread_attr_setstack(pthread_attr_t * attr, void * rs, size_t sz)
{
	OSThreadNotImplemented();
}

int pthread_attr_setstackaddr(pthread_attr_t * attr, void *rs)
{
	OSThreadNotImplemented();
}




/**********************************************************************************/
#pragma mark General Thread APIs

mach_port_t pthread_mach_thread_np(pthread_t th)
{
	OSThread* nt = GetOSThread(th);
	return nt->kernel_thread;
}

void __reserved_OSThread_1()
{
	OSHalt("Called a reserved function");
}

int pthread_detach(pthread_t thread)
{
	OSThreadNotImplemented();
}

int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param * param)
{
	return 0;
}

int pthread_equal(pthread_t one, pthread_t two) {
	/*
		XXX: should probably do a pid check or something.
	 */
	
	OSThread* t1 = GetOSThread(one);
	OSThread* t2 = GetOSThread(two);
	
	if (gThreadLog) {
		OSThreadLog("pthread_equal: %p %p", t1, t2);
	}
	
	if (t1 == t2) {
		return 1;
	}
	else {
		return 0;
	}
}

int pthread_getconcurrency(void)
{
	OSThreadNotImplemented();
}

int pthread_getschedparam(pthread_t thread, int * rs, struct sched_param * prm)
{
	OSThreadNotImplemented();
}

int pthread_once(pthread_once_t *st, void (*ent)(void))
{
	OSThreadOnce* once = (OSThreadOnce*)st;
	
	if (gThreadLog) {
		OSThreadLog("pthread_once: @ %p, sig %p", st, once->sig);
	}
	
	long sig = once->sig;
	
	if (sig == _PTHREAD_ONCE_SIG_init) {
		lll_lock(&(once->control));
		if (once->sig == _PTHREAD_ONCE_SIG_init) {
            (*ent)();
            once->sig = 0;
        }
		lll_unlock(&(once->control));
	}
	
	return 0;
}

pthread_t pthread_self(void) 
{
	return (pthread_t)OSGetCurrentThread();
}

int	pthread_main_np(void)
{
	/*
		Returns non-zero if the current thread is the main thread
		Main thread has (P/T)ID=0.
	 */
	
	OSThread* current_thread = (OSThread*)OSGetCurrentThread();
	
	if (current_thread->pid == 0) {
		return 1;
	}
	else {
		return 0;
	}
}


/**********************************************************************************/
#pragma mark Thread Mutex Attributes

int pthread_mutexattr_destroy(pthread_mutexattr_t *mutexattr)
{
	return 0;
	OSThreadNotImplemented();
}

int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *mutexattr, int *rs)
{
	OSThreadNotImplemented();
}

int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *mutexattr, int *rs)
{
	OSThreadNotImplemented();
}

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *mutexattr, int *rs)
{
	OSThreadNotImplemented();
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *mutexattr, int *rs)
{
	OSThreadNotImplemented();
}

int pthread_mutexattr_init(pthread_mutexattr_t * mutexattr)
{
	OSMutexAttributes* attr = GetOSMutexAttributes(mutexattr);
	
	attr->type = PTHREAD_MUTEX_DEFAULT;
	attr->sig = 0;
	
	return 0;
}

int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *mutexattr, int rs)
{
	OSThreadNotImplemented();
}

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *mutexattr, int rs)
{
	OSThreadNotImplemented();
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t *mutexattr, int rs)
{
	/* XXX: POSIX behaviour */
	
	return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *mutexattr, int rs)
{
	OSMutexAttributes* attr = GetOSMutexAttributes(mutexattr);
	
	if (gThreadLog) {
		OSThreadLog("pthread_mutexattr_settype(%p, %d)", mutexattr, rs);
	}
	
	attr->type = rs;
	
	return 0;
}




/**********************************************************************************/
#pragma mark Thread Mutexes

void _OSProcessMutexSignal(OSMutex* mutex)
{
	OSThread* current_thread = (OSThread*)OSGetCurrentThread();
	pid_t current_tid = current_thread->pid;
	
	/* init signals */
	if (mutex->sig == _PTHREAD_MUTEX_SIG_init) {
		if (gThreadLog) {
			OSThreadLog("_OSProcessMutexSignal(%p): signaled _PTHREAD_MUTEX_SIG_init", mutex);
		}
		
		mutex->sig = 0;
		mutex->owner = current_tid;
		mutex->type = PTHREAD_MUTEX_NORMAL;
	}
	else if (mutex->sig == _PTHREAD_RECURSIVE_MUTEX_SIG_init) {
		if (gThreadLog) {
			OSThreadLog("_OSProcessMutexSignal(%p): signaled _PTHREAD_RECURSIVE_MUTEX_SIG_init", mutex);
		}
		
		mutex->sig = 0;
		mutex->owner = current_tid;
		mutex->type = PTHREAD_MUTEX_RECURSIVE;
	}
	else if (mutex->sig == _PTHREAD_ERRORCHECK_MUTEX_SIG_init) {
		if (gThreadLog) {
			OSThreadLog("_OSProcessMutexSignal(%p): signaled _PTHREAD_RECURSIVE_MUTEX_SIG_init", mutex);
		}
		OSHalt("osprocessmutexsingal errorchceck");
		mutex->sig = 0;
		mutex->owner = current_tid;
		mutex->type = PTHREAD_MUTEX_ERRORCHECK;
	}
}

int pthread_mutex_lock(pthread_mutex_t *pmutex)
{	
	OSMutex* mutex = GetOSMutex(pmutex);
	_OSProcessMutexSignal(mutex);
	
	uint32_t type = mutex->type;
	OSThread* current_thread = (OSThread*)OSGetCurrentThread();
	pid_t current_tid = current_thread->pid;
	
	if (type == PTHREAD_MUTEX_NORMAL) {
		lll_lock(&(mutex->lll));
	}
	else if (type == PTHREAD_MUTEX_RECURSIVE) {
		if (mutex->owner == current_tid) {
			/* This thread owns the mutex */
			
			lll_lock(&recursive_op_lock);
			
			mutex->count += 1;
			if (mutex->count == 1) {
				lll_lock(&(mutex->lll));
			}
			
			lll_unlock(&recursive_op_lock);
		}
		else {
			OSHalt("pthread_mutex_lock: PTHREAD_MUTEX_RECURSIVE locking nonowned mutex");
		}
	}
	else if (type == PTHREAD_MUTEX_ERRORCHECK) {
		int mtr = lll_trylock(&(mutex->lll));
		
		if (mtr && mutex->owner == current_tid)
		{
			OSLog("mutex_errorcheck: EDEADLK");
			return -EDEADLK;
		}
		
		//OSHalt("pthread_mutex_lock: Lock type %d (PTHREAD_MUTEX_ERRORCHECK) not implemented %d", type, mtr);
	}
	else {
		OSHalt("pthread_mutex_lock: Lock type %d not implemented", type);
	}
	
	return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
	OSThreadLog("trylock");
	return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *pmutex)
{
	OSMutex* mutex = GetOSMutex(pmutex);
	
	uint32_t type = mutex->type;
	OSThread* current_thread = (OSThread*)OSGetCurrentThread();
	pid_t current_tid = current_thread->pid;
	
	if (type == PTHREAD_MUTEX_NORMAL) {
		lll_unlock(&(mutex->lll));
	}
	else if (type == PTHREAD_MUTEX_RECURSIVE) {
		if (mutex->owner == current_tid) {
			/* This thread owns the mutex */
			lll_lock(&recursive_op_lock);
			
			if (mutex->count == 0) {
				lll_unlock(&recursive_op_lock);
				return -EPERM;
			}
			else if (mutex->count == 1) {
				lll_unlock(&(mutex->lll));
			}
			
			mutex->count -= 1;
			lll_unlock(&recursive_op_lock);
		}
		else {
			OSHalt("pthread_mutex_unlock: PTHREAD_MUTEX_RECURSIVE unlocking nonowned mutex");
		}
	}
	else if (type == PTHREAD_MUTEX_ERRORCHECK) {
		lll_unlock(&(mutex->lll));
	}
	else {
		OSHalt("pthread_mutex_unlock: Lock type %d not implemented", type);
	}
	
	return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
	//OSThreadLog("pthread_mutex_destroy!");
	//OSThreadNotImplemented();
	
	return 0;
}

int pthread_mutex_getprioceiling(const pthread_mutex_t * mutex, int * rs) {
	OSThreadNotImplemented();
}

int pthread_mutex_init(pthread_mutex_t * pmutex, const pthread_mutexattr_t * rs) {
	OSMutex* mutex = GetOSMutex(pmutex);
	OSThread* current_thread = (OSThread*)OSGetCurrentThread();
	pid_t current_tid = current_thread->pid;
	
	if (gThreadLog) {
		OSThreadLog("pthread_mutex_init(%p, %p)", pmutex, rs);
	}
	
	/*
		Uncomment this to force all mutexes to be normal
		Don't do this unless you know what you're doing.
	 */
	//mutex->sig = _PTHREAD_MUTEX_SIG_init;
	
	if (rs == NULL) {
		mutex->type = PTHREAD_MUTEX_NORMAL;
		mutex->count = 0;
	}
	else {
		/* get attrs */
		OSMutexAttributes* attr = GetOSMutexAttributes(rs);
		mutex->type = attr->type;
	}
	
	mutex->lll = 0;
	mutex->owner = current_tid;
	
	return 0;
}

int pthread_mutex_setprioceiling(pthread_mutex_t *mutex, int cl, int * rs) {
	OSThreadNotImplemented();
}




/**********************************************************************************/
#pragma mark Thread Creation

int _OSThreadEntry(void* self, void* arg)
{
	/*
		Entry point for this thread.
		(self points to _OSThreadEntry as it's in R0)
	 */
	OSThread* thread = arg;
	
	/*
		Set the TLS area for the newly spawned thread.
	 */
	set_tls$LINUX(thread);
	if (OSGetCurrentThread() == 0) {
		OSHalt("OSThread: failed to set TLS bit for the new thread");
	}
	
	//if (gThreadLog) {
		 OSThreadLog(" *** thread start, entry @ %p", thread->thread_entry);
	//}
	
	(thread->thread_entry)(thread->thread_arg);
	
	/* XXX: yep */
	OSThreadLog("thread spinning...");
	while (1) {}
}

/*
	Create a new thread and stuff.
	Layout:
		* OSThread structure
		* TLS (OS_TLS_SIZE)
		* Stack (variable size)
 */
int pthread_create(pthread_t * pthread,
				   const pthread_attr_t *attr,
				   void *(*start_routine)(void*),
				   void *arg)
{
	int stack_sz;
	
	if (attr != NULL) {
		OSThreadAttributes* tt = GetOSThreadAttributes(attr);
		stack_sz = tt->stack_size;
	}
	else {
		stack_sz = OS_DEFAULT_THREAD_STACK;
	}
	
	int tls_sz = OS_TLS_SIZE;
	int total_thread_sz = sizeof(OSThread) + tls_sz + stack_sz;
	OSThread* thread = malloc(total_thread_sz);
	
	void* stack_start = (void*)((uintptr_t)thread + sizeof(OSThread) + tls_sz);
	void* stack_end = (char*)stack_start + stack_sz;
	
	if (gThreadLog) {
		 OSThreadLog("OSThread @ %p {sz=%d, stk=%d, ent=%p}",
					 thread,
					 total_thread_sz,
					 stack_sz,
					 start_routine);
	}
	
	memset(stack_start, 0, stack_sz);
	
	pid_t new_pid;
	
	thread->top_stack = stack_end;
	thread->thread_entry = start_routine;
	thread->thread_arg = arg;
	
	new_pid = clone$LINUX(_OSThreadEntry,
						  stack_end,
						  LX_SIGCHLD | LX_CLONE_FS | LX_CLONE_FILES | LX_CLONE_SIGHAND | LX_CLONE_VM | LX_CLONE_THREAD,
						  thread);
	
	thread->pid = new_pid;

	*pthread = (pthread_t)thread;
	
	return 0;
}