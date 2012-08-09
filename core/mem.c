/*
 * Core Framework: Libc: Memory: mem.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Low level memory allocation routines.
 */

#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include "_lnk_log_c.h"
#include <sys/mman.h>

extern void _ZN3lnk4ldbg11printNumberEPKci(const char* ch, int num);

/* Syscall */
extern void* brk$LINUX(void* end);

uint8_t* __Break = (void*)0;
#define  SBRK_ALIGN  32

size_t getpagesize$long(void) {
	/*
	 XXX: ugly hack
	 */
	return (size_t)4096;
}

void *
reallocf(void *ptr, size_t size)
{
	/*
		XXX
	 
		The reallocf function is identical to the realloc function,
		except that it will free the passed pointer when the requested
		memory cannot be allocated.
	 
		This is a specific API designed to ease the problems with traditional
		coding styles for realloc causing memory leaks in libraries.
	 */
	
	return realloc(ptr, size);
}

int mincore(const void *addr, size_t length, char *vec)
{
	*vec = MINCORE_INCORE;
	return 0;
}

int	getpagesize(void) {
	/*
		XXX: ugly hack
	 */
	return 4096;
}

/*
	Break functions for dynamic memory stuff.
*/

void* brk(void* end_data)
{
    uint8_t* new_brk =
	brk$LINUX(end_data);
	
    if (new_brk != end_data)
        return (void*)-1;
	
    __Break = new_brk;
	
	return 0;
}

void* sbrk(int increment)
{
	_ZN3lnk4ldbg11printNumberEPKci("WARNING *** 'sbrk()' CALLED WITH INCR = ", increment);
	
    uint8_t*  start;
    uint8_t*  end;
    uint8_t*  new_brk;
	
	/* get initial value from the kernel */
    if (!__Break)
        __Break = brk$LINUX((void*)0);
	
    start = (uint8_t*)(((long)__Break + SBRK_ALIGN-1) & ~(SBRK_ALIGN-1));
	end   = start + increment;
	
	new_brk = brk$LINUX(end);
	
    if (new_brk == (void*)-1)
        return new_brk;
    else if (new_brk < end)
    {
		errno = ENOMEM;
        return (void*)-1;
	}
	
    __Break = new_brk;
	return start;
}
