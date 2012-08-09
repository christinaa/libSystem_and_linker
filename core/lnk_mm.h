/*
 * Core Framework: Linker: lnk_mm.h
 * Copyright (c) 2012 Christina Brooks
 *
 * Memory management support routines for the linker.
 */

#ifndef core_lnk_mm_h
#define core_lnk_mm_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace lnk {
	namespace mm {
		/* reserve a memory range */
		void* reserve(size_t sz);
		
		/* wire in a file range */
		void* wire(int fd, void* addr, size_t vmsize, uint32_t fileoff);
		
		/* wire anonymous */
		void* wire_anon(void* addr, size_t vmsize);
	}
}

#endif
