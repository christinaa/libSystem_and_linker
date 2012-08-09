/*
 * Core Framework: Linker: lnk_api.cpp
 * Copyright (c) 2012 Christina Brooks
 *
 * API for the dynamic linker.
 */

/*
 * Dyld API.
 * Needs to be dyld compatible.
 * I need to rewrite all this at some point.
 */

#include "lnk.h"
#include <ctype.h>
#include <math.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include "OSLog.h"
#include <mach-o/ldsyms.h>
#include <string.h>
#include <mach/mach.h>
#include <mach-o/loader.h>
#include <mach-o/ldsyms.h>
#include <mach-o/nlist.h> 
#include <mach-o/reloc.h>
#include <mach-o/dyld.h>

struct dyld_image_info {
	const struct mach_header*	imageLoadAddress;	/* base address image is mapped into */
	const char*					imageFilePath;		/* path dyld used to load the image */
	uintptr_t					imageFileModDate;	/* time_t of image file */
	/* if stat().st_mtime of imageFilePath does not match imageFileModDate, */
	/* then file has been modified since dyld loaded it */
};

enum dyld_image_states
{
	dyld_image_state_mapped					= 10,		// No batch notification for this
	dyld_image_state_dependents_mapped		= 20,		// Only batch notification for this
	dyld_image_state_rebased				= 30, 
	dyld_image_state_bound					= 40,
	dyld_image_state_dependents_initialized	= 45,		// Only single notification for this
	dyld_image_state_initialized			= 50,
	dyld_image_state_terminated				= 60		// Only single notification for this
};

typedef const char* (*dyld_image_state_change_handler)(enum dyld_image_states state, uint32_t infoCount, const struct dyld_image_info info[]);

extern "C" struct mach_header* _NSGetMachExecuteHeader();


typedef void (*AddImageHandler)(const struct mach_header* mh, intptr_t vmaddr_slide);

typedef struct {
	dyld_image_state_change_handler fn;
	enum dyld_image_states state;
	bool batch;
} ChangeHandler;

static AddImageHandler sAddHandlers[1024];
static ChangeHandler sChangeHandlers[1024];

static uint32_t sAddCount = 0;
static uint32_t sChangeCount = 0;

extern "C" {
	void _dyld_register_func_for_add_image(AddImageHandler hn);
	void _dyld_register_func_for_remove_image(void (*func)(const struct mach_header* mh, intptr_t vmaddr_slide));
	void dyld_register_image_state_change_handler(enum dyld_image_states state, bool batch, dyld_image_state_change_handler handler);
}

namespace lnk {
	void runBatchNotificationsWithHandler(ChangeHandler hn)
	{
		OSLog("runBatchNotificationsWithHandler(%d, %d, %p)", hn.state, hn.batch, hn.fn);
		
		
		dyld_image_info infos[1024];
		uint32_t count = 0;
		
		Image* im;
		im = lnk::firstImage();
		while (im != NULL) {
			if (im->getImageType() != kImageTypeMachO)
			{
				lnk::halt("(im->getImageType() != kImageTypeMachO)");
			}
			
			/* XXX: check if mach object */
			MachObject* obj = (MachObject*)im;
			
			infos[count].imageLoadAddress = obj->getMachHeader();
			infos[count].imageFilePath = "";
			
			/* move on */
			im = im->nextImage();
			count += 1;
		}

		/* main image needs to be here too */
		MachObject* obj = (MachObject*)(lnk::mainExecutableImage());
		infos[count].imageLoadAddress = obj->getMachHeader();
		infos[count].imageFilePath = "";
		count += 1;
		
		dyld_image_state_change_handler fn = hn.fn;
		fn(dyld_image_state_bound, count, infos);
	}
	
	void runBoundNotifications() {
		
		
		/* run the initializers */
		for (int i = 0; i < sChangeCount; i++) {
			runBatchNotificationsWithHandler(sChangeHandlers[i]);
		}
	}
	
	void runAddImageHandler(AddImageHandler fn) {
		Image* im;
		im = lnk::firstImage();
		while (im != NULL) {
			/* XXX: check if mach object */
			MachObject* obj = (MachObject*)im;
			
			OSLog("calling add handler @ %p, image %s", fn, im->getShortName());
			fn(obj->getMachHeader() ,obj->getSlide());
			
			/* move on */
			im = im->nextImage();
		}
	}
}

uint32_t
_dyld_image_count(void) {
	uint32_t count = 0;
	
	Image* im;
	im = lnk::firstImage();
	while (im != NULL) {
		count++;
		/* move on */
		im = im->nextImage();
	}
	
	return count;
}

const struct mach_header*
_dyld_get_image_header(uint32_t image_index)
{
	uint32_t count = 0;
	
	Image* im;
	im = lnk::firstImage();
	while (im != NULL) {
		if (count == image_index) {
			MachObject* obj = (MachObject*)im;
			return obj->getMachHeader();
		}
		
		count++;
		/* move on */
		im = im->nextImage();
	}
	
	return NULL;
}

intptr_t
_dyld_get_image_vmaddr_slide(uint32_t image_index)
{
	uint32_t count = 0;
	
	Image* im;
	im = lnk::firstImage();
	while (im != NULL) {
		if (count == image_index) {
			MachObject* obj = (MachObject*)im;
			return obj->getSlide();
		}
		
		count++;
		/* move on */
		im = im->nextImage();
	}
	
	return NULL;
}

void
dyld_register_image_state_change_handler(enum dyld_image_states state, bool batch, dyld_image_state_change_handler handler)
{
	OSLog("dyld_register_image_state_change_handler(%d, %d, %p)", state, batch, handler);
	
	sChangeHandlers[sChangeCount].fn = handler;
	sChangeHandlers[sChangeCount].state = state;
	sChangeHandlers[sChangeCount].batch = batch;
	sChangeCount += 1;
	
	lnk::runBatchNotificationsWithHandler(sChangeHandlers[sChangeCount-1]);
}

struct mach_header* _NSGetMachExecuteHeader() {
	return (struct mach_header*)lnk::mainHeader();
}

int dladdr(const void *ptr , Dl_info *dl)
{
	Image* im;
	Image* foundIn = NULL;
	
	im = lnk::firstImage();
	while (im != NULL) {
		MachObject* obj = (MachObject*)im;
		uintptr_t start = 0;
		uintptr_t end = 0;
		uintptr_t uptr = (uintptr_t)ptr;
		
		obj->getTextSegmentBounds(&start, &end);
		
		if (uptr <= end && uptr >= start)
		{
			/* we've got a match */
			foundIn = im;
			break;
		}
		
		im = im->nextImage();
	}
	
	if (foundIn != NULL)
	{
		OSLogLib("linkerAPI", "dladdr(%p): ptr found in '%s'", ptr, im->getShortName());
		
		dl->dli_sname = NULL;
		dl->dli_saddr = NULL;
		
		dl->dli_fname = im->filePath();
		dl->dli_fbase = (void*)im->getSlide();
		
		/* non-zero on success */
		return 1; 
	}
	else
	{
		OSLogLib("linkerAPI", "dladdr(%p): ptr not found", ptr);
		
		dl->dli_sname = NULL;
		dl->dli_saddr = NULL;
		
		/* 0 on error */
		return 0; 
	}
}

void*
dlopen(const char *__path, int __mode) {
	MachObject* dlib;
	bool already_loaded;
	
	dlib = (MachObject*)lnk::loadLibraryEx(__path, &already_loaded);
	
	if (dlib == lnk::firstImage()) {
		OSLogLib("linkerAPI", "dlopen(%s, %d) = 0x0, not loaded", __path, __mode);
		return NULL;
	}
	
	if (already_loaded) {
		OSLogLib("linkerAPI", "dlopen(%s, %d) = [re]loaded '%s'", __path, __mode, dlib->getShortName());
	}
	else {
		/* bootstrap lib */
		dlib->doBindSymbols();
		dlib->doInitialize();
		
		OSLogLib("linkerAPI", "dlopen(%s, %d) = loaded '%s'", __path, __mode, dlib->getShortName());
	}
	
	return (void*)dlib;
}

int
dlclose(void *__handle) {
	OSHaltNotImplemented();
}

bool
dlopen_preflight(const char *__path) {
	OSHaltNotImplemented();
}

void*
dlsym(void *__handle, const char *__symbol) {
	MachObject* dlib = (MachObject*)__handle;
	Symbol sym;
	bool ret;
	
	char* thing = (char*)calloc(1, strlen(__symbol) + 3);
	thing[0] = '_';
	
	strcpy(thing+1, __symbol);
	
	ret = 
	dlib->findExportedSymbol(thing, &sym);
	
	if (ret) {
		uintptr_t addr = 
		((Image*)sym.inImage)->exportedSymbolAddress(&sym);
		
		OSLogLib("linkerAPI", "dlsym(DL[%s], %s) = %p", dlib->getShortName(), thing, addr);
		
		free(thing);
		
		return (void*)addr;
	}
	
	OSLogLib("linkerAPI", "dlsym(DL[%s], %s) = 0x0, symbol not found", dlib->getShortName(), thing);
	
	free(thing);
	
	return NULL;
}

char*
dlerror() {
	OSHaltNotImplemented();
}

void _dyld_register_func_for_add_image(AddImageHandler hn) {
	OSLog("_dyld_register_func_for_add_image(%p)", hn);
	sAddHandlers[sAddCount] = hn;
	sAddCount += 1;
	
	lnk::runAddImageHandler(hn);
}

void _dyld_register_func_for_remove_image(void (*func)(const struct mach_header* mh, intptr_t vmaddr_slide)) {
	OSLog("_dyld_register_func_for_remove_image(%p)", func);
}