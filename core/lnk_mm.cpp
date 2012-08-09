/*
 * Core Framework: Linker: lnk_mm.cpp
 * Copyright (c) 2012 Christina Brooks
 *
 * Memory management support routines for the linker.
 */

#include "lnk_mm.h"
#include <sys/mman.h>
#include <string.h>

namespace lnk {
	namespace mm {
		/* reserve a memory range */
		void* reserve(size_t sz) {
			void* rs = 
			mmap(NULL,
				 sz,
				 PROT_READ | PROT_WRITE,
				 MAP_ANON | MAP_PRIVATE,
				 0,
				 0);
			
			return rs;
		}
		
		void* wire_anon(void* addr, size_t vmsize) {
			void* rs = 
			mmap(addr,
				 vmsize,
				 PROT_READ | PROT_WRITE,
				 MAP_ANON | MAP_PRIVATE | MAP_FIXED,
				 0,
				 0);
			
			return rs;
		}
		
		void* wire(int fd, void* addr, size_t vmsize, uint32_t fileoff) {
			return
			mmap(addr,
				 vmsize,
				 PROT_READ | PROT_WRITE | PROT_EXEC,
				 MAP_PRIVATE | MAP_FIXED,
				 fd,
				 fileoff);
		}
	}
}