/*
 * Core Framework: Libc: mmap.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Darwin mmap shim.
 */

#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>

/* Syscall */
extern void* mmap2$LINUX(void*, size_t, int, int, int, size_t);

/* kernel wants these */
#define LX_PROT_READ 0x1  
#define LX_PROT_WRITE 0x2  
#define LX_PROT_EXEC 0x4  
#define LX_PROT_SEM 0x8  
#define LX_PROT_NONE 0x0  
#define LX_PROT_GROWSDOWN 0x01000000  
#define LX_PROT_GROWSUP 0x02000000  
#define LX_MAP_SHARED 0x01  
#define LX_MAP_PRIVATE 0x02  
#define LX_MAP_TYPE 0x0f  
#define LX_MAP_FIXED 0x10  
#define LX_MAP_ANONYMOUS 0x20  
extern void _ZN3lnk4ldbg11printNumberEPKci(const char* ch, int num);
#define MMAP2_SHIFT  12
/*
	Linux Mmap Function
	This is here just in case I ever need to link against
	the linux variant. Or for some other compat stuff.
 */
void* mmap$LINUX(void* addr,
				 size_t size,
				 int  prot,
				 int  flags,
				 int  fd, 
				 off_t offset)
{
	/* meh, don't know what this does */
	if (offset & ((1UL << MMAP2_SHIFT)-1)) {
		errno = EINVAL;
		return MAP_FAILED;
	}
	
	return mmap2$LINUX(addr, size, prot, flags, fd, (size_t)offset >> MMAP2_SHIFT);
}

/*
	 Darwin Mmap Function:
	 mmap(void *, size_t, int, int, int, off_t)
	 
		* prot can be left the same
		* flags need to be fixed up before making the syscall
 */
void* mmap(void* addr,
		   size_t size,
		   int  prot,
		   int  flags,
		   int  fd, 
		   off_t offset)
{	
	/* meh, don't know what this does */
	if (offset & ((1UL << MMAP2_SHIFT)-1)) {
		errno = EINVAL;
		return MAP_FAILED;
	}

	int uflags = 0;
	void* mret = 0;
	
	/* flag fixup */
	if (flags & MAP_ANON)
		uflags |= LX_MAP_ANONYMOUS;
	if (flags & MAP_SHARED)
		uflags |= LX_MAP_SHARED;
	if (flags & MAP_FIXED)
		uflags |= LX_MAP_FIXED;
	if (flags & MAP_PRIVATE)
		uflags |= LX_MAP_PRIVATE;

	mret = mmap2$LINUX(addr, size, prot, uflags, fd, (size_t)offset >> MMAP2_SHIFT);
	
	if ((int)mret < 4096) {
		_ZN3lnk4ldbg11printNumberEPKci("MAP FAILED!!!!!! - ", (int)mret);
	}
	
	return mret;
}
