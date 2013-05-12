/*
 * Core Framework: OSInitializer.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Runtime starter.
 */

#include <stdio.h>

extern void mach_init(void); /* mach */
extern void _OSInitializeMemoryZones(void); /* Libc/Memory/malloc_zone.c */
extern void _OSThreadInitialize(void);
extern void _OSLogInit(void);
extern void _time_init(void);
extern void FakeLocaleInit(void);

/*
 * Kick off the internal initializers for dynamic libs in Core.
 * This should be called after the Auxiliary Core libraries were linked in.
 */
void OSInitializeAuxiliaryRuntime(void) {
	
}

/*
 * Kick off the critical static libs in Core.
 * This should be called as soon as lnk starts up.
 */
void OSInitializeMainRuntime(void) {
	_time_init();
	_OSLogInit();
	_OSInitializeMemoryZones();
	mach_init();
	_OSThreadInitialize();
	FakeLocaleInit();
}

// newline! \n

