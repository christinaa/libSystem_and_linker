/*
 * Core Framework: Linker: lnk.h
 * Copyright (c) 2012 Christina Brooks
 *
 * Header for the lnk.cpp thing.
 */

#ifndef core_lnk_h
#define core_lnk_h

#include <mach/mach.h>
#include <mach-o/loader.h>
#include <mach-o/ldsyms.h>
#include <mach-o/nlist.h> 
#include <mach-o/reloc.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "macho.h"
#include "lnk_macho.h"
#include "lnk_image.h"

#define LIBSYSTEM_PATH "/usr/lib/libSystem.B.dylib"
#define LIBGCC_PATH "/usr/lib/libgcc_s.1.dylib"

namespace lnk {
	/* getters */
	const macho_header* lnkHeader();
	const intptr_t lnkSlide();
	const macho_header* mainHeader();
	
	/* loader */
	Image* loadLibrary(const char* path);
	Image* loadLibraryEx(const char* path, bool* already_loaded);
	
	/* util */
	void log(const char* format, ...);
	void warn(const char* format, ...);
	void halt(const char* format, ...) __attribute__((noreturn)); 
	char* safe_symbol_name(char* sym);
	
	void runBoundNotifications();
	
	Image* firstImage();
	Image* mainExecutableImage();
	
	/* resolve */
	uintptr_t resolve(const char* symbolName,
					  uint8_t symboFlags,
					  int libraryOrdinal,
					  Image** foundImage,
					  Image* requiredBy);
	
	void callInitializer(Initializer fn, Image* image);
	
	/* entry */
	uintptr_t
	_main(const macho_header* mainExecutableMH,
		  uintptr_t mainExecutableSlide,
		  int argc,
		  const char* argv[],
		  const char* envp[],
		  const char* apple[]);
}

#endif
