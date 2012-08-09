/*
 * Core Framework: Linker: lnk_debug
 * Copyright (c) 2012 Christina Brooks
 *
 * Linker debugging code. 
 */

#ifndef core_lnk_debug_h
#define core_lnk_debug_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

namespace lnk {
	namespace ldbg {
		void printText(const char* text);
		void printNumber(const char* desc, int num);
	}
	
	namespace dbg {
		
	}
}

#endif
