/*
 * Core Framework: Libc: Memory: malloc_zone.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Support for malloc zones.
 * Luckily, dlmalloc has mspaces, which are like alloc zones.
 * So lets use them!
 */

#include <stdio.h>
#include <unistd.h>
#include <malloc/malloc.h>
#include "dlmalloc.h"
#include "OSThread.h"
#include <stdarg.h>
#include "OSLog.h"

#define MAX_ZONES 1024

typedef struct {
	/* base */
    malloc_zone_t basic_zone;
	
	/* our shit */
    mspace memory_space; /* dlmalloc's mspace */
	boolean_t global_zone; /* if this is set, use global funcs */
	boolean_t registred; /* if true, then this is being used */
} OSMemoryZone;

/* stuff */
uint32_t zone_count = 0;
OSLowLock malloc_lock;
OSMemoryZone zones[MAX_ZONES];
OSMemoryZone* default_zone = NULL;

/* Hacky mspace function to check if a mpsace has a pointer in it */
int XXX_mspace_has_pointer(mspace msp, void* ptr);

/*
 * Other malloc stuff.
 */
#define PAGEALIGN(_v) (((_v) + PAGE_SIZE- 1) & ~(PAGE_SIZE - 1))

/*
 * OSMemoryZone
 */
OSMemoryZone* _OSGetFirstAvailableMemoryZone(void) {
	lll_lock(&malloc_lock);
	
	OSMemoryZone* osm = NULL;
	for (int i = 0; i < MAX_ZONES; i++) {
		OSMemoryZone* tosm = &zones[i];
		if (tosm->registred == FALSE) {
			osm = tosm;
		}
	}
	
	if (osm != NULL) {
		osm->registred = TRUE;
	}
	else {
		OSHalt("out of avail malloc zones (max: %d)", MAX_ZONES);
	}
	
	lll_unlock(&malloc_lock);
	
	return (OSMemoryZone*)osm;
}

void _OSInitializeMemoryZones(void) {
	/* create the default memory zone */
	
	if (default_zone != NULL) {
		OSHalt("default_zone already present!");
	}
	
	/* 0xfee1dead is the magic that stops the create thing from creating a mspace */
	default_zone = (OSMemoryZone*)malloc_create_zone(0, 0xfee1dead);
	default_zone->global_zone = TRUE;
	default_zone->basic_zone.zone_name = "DefaultMallocZone";
	
	OSLog("_OSInitializeMemoryZones: all set, %d zones inited, default zone @ %p", MAX_ZONES, default_zone);
}

malloc_zone_t *
malloc_zone_from_ptr(const void *ptr)
{
	OSLog("malloc_zone_from_ptr(%p): searching ... ", ptr);
	
	lll_lock(&malloc_lock);
	
	OSMemoryZone* osm = NULL;
	for (int i = 0; i < MAX_ZONES; i++) {
		OSMemoryZone* tosm = &zones[i];
		
		if (tosm->registred && tosm->memory_space != NULL) {
			if (XXX_mspace_has_pointer(tosm->memory_space, (void*)ptr))
			{
				osm = tosm;
				break;
			}
		}
	}
	
	lll_unlock(&malloc_lock);
	
	if (osm == NULL) {
		osm = default_zone;
		OSLog("malloc_zone_from_ptr(%p): no hits, returinig default_zone", ptr);
	}
	else {
		OSLog("malloc_zone_from_ptr(%p): found zone [%p] ", ptr, osm);
	}
	
	return (malloc_zone_t*)osm;
}

size_t malloc_size(const void* ptr) {
	/*
		Docs say that usinng usable_size is considered to
		be a bad practice. I don't really care, it does the job.
	 */
	return malloc_usable_size((void*)ptr);
}

size_t malloc_good_size(size_t size) {
	return PAGEALIGN(size);
}

void malloc_set_zone_name(malloc_zone_t *zone, const char *name) {
	OSLog("malloc_set_zone_name(%p, %s)", zone, name);
}

void malloc_printf(const char* format, ...) 
{
	printf("[Malloc]: ");
	
	va_list	list;
	va_start(list, format);
	vfprintf(stdout, format, list);
	va_end(list);
	
	fflush(stdout);
}

unsigned int malloc_zone_batch_malloc(malloc_zone_t *zone, size_t size, void **results, unsigned int num_requested)
{
	OSHalt("malloc_zone_batch_malloc");
}

void __reserved_malloc_1() {
	OSHalt("called a reserved function");
} 


/*
	Default zone handlers which work using mspaces.
 */
malloc_zone_t *
Impl_malloc_create_zone(vm_size_t start_size, unsigned flags) {
	OSHalt("Impl_malloc_create_zone is not meant to be called!");
}

void
Impl_malloc_destroy_zone(malloc_zone_t *zone) {
	
}

void *
Impl_malloc_zone_malloc(malloc_zone_t *zone, size_t size) {
	OSMemoryZone* osm = (OSMemoryZone*)zone;
	if (osm->global_zone) {
		return malloc(size);
	}
	else {
		return mspace_malloc(osm->memory_space, size);
	}
}

void *
Impl_malloc_zone_calloc(malloc_zone_t *zone, size_t num_items, size_t size) {
	OSMemoryZone* osm = (OSMemoryZone*)zone;
	if (osm->global_zone) {
		return calloc(num_items, size);
	}
	else {
		return mspace_calloc(osm->memory_space, num_items, size);
	}
}

void *
Impl_malloc_zone_valloc(malloc_zone_t *zone, size_t size) {
	OSMemoryZone* osm = (OSMemoryZone*)zone;
	if (osm->global_zone) {
		return valloc(size);
	}
	else {
		OSHalt("Impl_malloc_zone_valloc for mspaces isn't implemented yet!");
	}
}

void *
Impl_malloc_zone_realloc(malloc_zone_t *zone, void *ptr, size_t size) {
	OSMemoryZone* osm = (OSMemoryZone*)zone;
	if (osm->global_zone) {
		return realloc(ptr, size);
	}
	else {
		return mspace_realloc(osm->memory_space, ptr, size);
	}
}

void *
Impl_malloc_zone_memalign(malloc_zone_t *zone, size_t alignment, size_t size) {
	OSMemoryZone* osm = (OSMemoryZone*)zone;
	if (osm->global_zone) {
		return memalign(alignment, size);
	}
	else {
		return mspace_memalign(osm->memory_space, alignment, size);
	}
}

void
Impl_malloc_zone_free(malloc_zone_t *zone, void *ptr) {
	OSMemoryZone* osm = (OSMemoryZone*)zone;
	if (osm->global_zone) {
		return free(ptr);
	}
	else {
		return mspace_free(osm->memory_space, ptr);
	}
}


/*
	Abstract. 
 */
malloc_zone_t *
malloc_create_zone(vm_size_t start_size, unsigned flags) {
	OSMemoryZone* osm = _OSGetFirstAvailableMemoryZone();
	
	/* mkay, get default impls */
	osm->basic_zone.calloc = (void*)Impl_malloc_zone_calloc;
	osm->basic_zone.free = (void*)Impl_malloc_zone_free;
	osm->basic_zone.malloc = (void*)Impl_malloc_zone_malloc;
	osm->basic_zone.valloc = (void*)Impl_malloc_zone_valloc;
	osm->basic_zone.memalign = (void*)Impl_malloc_zone_memalign;
	osm->basic_zone.realloc = (void*)Impl_malloc_zone_realloc;
	
	if (flags != 0xfee1dead) {
		/* make a mspace */
		osm->memory_space = create_mspace(start_size, FALSE);
	}
	
	OSLog("malloc_create_zone: created zone {size=%d, space=%p, addr=%p}",
		  start_size,
		  osm->memory_space,
		  osm);
	
	return (malloc_zone_t*)osm;
}

void
malloc_destroy_zone(malloc_zone_t *zone) {
	OSHalt("malloc_destroy_zone: not yet implemented!");
}

malloc_zone_t *
malloc_default_zone(void) {
	return (malloc_zone_t*)default_zone;
}

void *
malloc_zone_malloc(malloc_zone_t *zone, size_t size) {
	return zone->malloc(zone, size);
}

void *
malloc_zone_calloc(malloc_zone_t *zone, size_t num_items, size_t size) {
	return zone->calloc(zone, num_items, size);
}

void *
malloc_zone_valloc(malloc_zone_t *zone, size_t size) {
	return zone->valloc(zone, size);
}

void *
malloc_zone_realloc(malloc_zone_t *zone, void *ptr, size_t size) {
	return zone->realloc(zone, ptr, size);
}

void *
malloc_zone_memalign(malloc_zone_t *zone, size_t alignment, size_t size) {
	return zone->memalign(zone, alignment, size);
}

void
malloc_zone_free(malloc_zone_t *zone, void *ptr) {
	return zone->free(zone, ptr);
}