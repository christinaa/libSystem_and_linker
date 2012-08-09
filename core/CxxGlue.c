/*
 * Core Framework: cxxglue.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Glue code to get C++ code inside the linker to work when needed.
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "OSLog.h"

#include <mach/mach.h>
#include <mach-o/loader.h>
#include <mach-o/ldsyms.h>
#include <mach-o/nlist.h> 
#include <mach-o/reloc.h>

extern void _ZN3lnk4haltEPKcz(const char* format, ...) __attribute__((noreturn));

#define __dylinker__

void _HandleUndefinedSymbol(uintptr_t pc, uintptr_t lr) {
	OSHalt("Called an unresolved symbol {lr=%p}.", lr);
}

/* operator new */
void* _Znwm(size_t size) {
	return malloc(size);
}

void abort(void)
{	
	printf("\n");
	OSHalt("abort() called");
}
