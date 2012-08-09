/*
 * Core Framework: Linker: Tester.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Test stuff.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "OSLog.h"

#include <mach/mach.h>
#include <mach-o/loader.h>
#include <mach-o/ldsyms.h>
#include <mach-o/nlist.h> 
#include <mach-o/reloc.h>
#include <mach-o/getsect.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void TesterStart(void) {
	//__msgtest();
	//OSHalt("Tester finished running!");
}