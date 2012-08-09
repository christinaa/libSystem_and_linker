/*
 * Core Framework: Linker: lnk.cpp
 * Copyright (c) 2012 Christina Brooks
 *
 * Linker runtime.
 */

#include "lnk.h"
#include "lnk_elf.h"
#include "OSLog.h"
#include <ctype.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <stdlib.h>

#define __cas__

#if defined(__cas__)
#include "lnk_cas.h"
#endif

/*
 * We populate a table with all the loaded images to
 * provide information to the kernel in case it wants to
 * do a core dump.
 */
#define LINK_TABLE_ADDR 0x80014000

typedef struct {
	uint32_t entry_count;
	size_t table_size;
} linker_image_table_header_t;

typedef struct {
	uintptr_t load_addr;
	size_t size;
	const char* name;
} linker_image_entry_t;

static linker_image_table_header_t* sKernLinkTable = 
(linker_image_table_header_t*)LINK_TABLE_ADDR;

void linker_image_table_add(uintptr_t load_addr, size_t size, const char* name)
{
	linker_image_entry_t* ee = 
	(linker_image_entry_t*)(((char*)sKernLinkTable) + sizeof(linker_image_entry_t));
	
	uint32_t ec = sKernLinkTable->entry_count;
	
	ee[ec].load_addr = load_addr;
	ee[ec].size = size;
	ee[ec].name = name;
	
	sKernLinkTable->entry_count += 1;
}

#define AUX_COUNT 6

extern "C" {
	extern int __Break;
	extern void __msgtest();
	
	void OSInitializeMainRuntime(void);
	void OSInitializeAuxiliaryRuntime(void);
	void TesterStart(void);
}

/*
 * Globals.
 */
static MachObject* sMainExecutable = NULL;
static MachObject* sDylinker = NULL;
static Image* sFirstImage = NULL;
static const macho_header* sMainHeader = NULL;
static bool sVerboseLnkLog = false;
static uint32_t sImageCount = 0;
static uint32_t sUnresolvedCount = 0;
static Image* sAuxLibs[AUX_COUNT];
extern "C" void _DeadBabe(void);
extern "C" char* gArgv[1024];
extern "C" int gArgc;

namespace lnk {

	const macho_header* mainHeader() {
		return sMainHeader;
	}
	
	Image* mainExecutableImage() {
		return sMainExecutable;
	}
	
	Image* firstImage() {
		return sFirstImage;
	}
	
	
	uint32_t get_u_time(void* wtf) {
		return clock();
	}
	
	/*
	 * Sanitize symbol names for printing.
	 * This makes sure the symbol name does not contain any weird
	 * characters that would screw things up.
	 */
	char* safe_symbol_name(char* sym) {
		while (!isalnum(*sym) &&
			   *sym != '\0' &&
			   *sym != '_' &&
			   *sym != '@')
		{
			sym++;
		}
		return sym;
	}
	
	void halt(const char* format, ...) 
	{
		OSTermSetTextColor(BRIGHT, RED, VT_DEFAULT);	
		printf(" *** lnk error:  ");
		OSTermSetTextColor(RESET, RED, VT_DEFAULT);
		
		va_list	list;
		va_start(list, format);
		vfprintf(stdout, format, list);
		va_end(list);
		
		printf("\n");
		OSTermResetAttributes();
		fflush(stdout);
		
		/* die */
		_exit(0);
	}
	
	void log(const char* format, ...) 
	{
		if (__SilentMode)
			return;
		
		OSTermSetTextColor(LOG_FLAG, LOG_LNK_COLOR, VT_DEFAULT);	
		printf("[dylinker]: ");
		OSTermResetAttributes();
		
		va_list	list;
		va_start(list, format);
		vfprintf(stdout, format, list);
		va_end(list);
		
		printf("\n");
		OSTermResetAttributes();
		fflush(stdout);
	}
	
	void warn(const char* format, ...) 
	{		
		//if (__SilentMode)
		//	return;
		
		OSTermSetTextColor(LOG_FLAG, YELLOW, VT_DEFAULT);
		printf("[dyldwarn]: ");
		OSTermResetAttributes();
		
		va_list	list;
		va_start(list, format);
		vfprintf(stdout, format, list);
		va_end(list);
		
		printf("\n");
		OSTermResetAttributes();
		fflush(stdout);
	}
	
	/*
		Resolve a symbol by its name from the image chain.
	 */
	uintptr_t resolve(const char* symbolName,
					  uint8_t symboFlags,
					  int libraryOrdinal,
					  Image** foundImage,
					  Image* requiredBy) 
	{
		Symbol sym;
		bool ret;
		
		if (strcmp(symbolName, "_open") == 0) {
			//symbolName = "_open$shim";
		}
		
		const char* ns;
		
		if (libraryOrdinal == BIND_SPECIAL_DYLIB_FLAT_LOOKUP || 1)
		{
			ns = "<flat_namespace>";
			
			/*
			 * Perform a flat resolution
			 */
			Image* im = sFirstImage;
			
			while (im != NULL) {
				/* see if the image contains the symbol */
				ret = 
				im->findExportedSymbol(symbolName, &sym);
				
				if (ret) {
					uintptr_t addr = 
					((Image*)sym.inImage)->exportedSymbolAddress(&sym);
					
					*foundImage = im;
					
					return addr;
				}
				
				/* move on */
				im = im->nextImage();
			}
			
			/* flat symbol not found */
		}
		else
		{
			Image* im = *foundImage;
			ns = im->getShortName();
			
			if (!im) {
				lnk::halt("oops");
			}
			else {
				ret = 
				im->findExportedSymbol(symbolName, &sym);
				
				if (ret) {
					uintptr_t addr = 
					((Image*)sym.inImage)->exportedSymbolAddress(&sym);
					return addr;
				}
				else {
					if (im == sDylinker)
					{
						/* maybe an aux? */
						for (int i = 0; i < AUX_COUNT; i++)
						{
							ret = 
							sAuxLibs[i]->findExportedSymbol(symbolName, &sym);
							
							if (ret) {
								uintptr_t addr = 
								
								((Image*)sym.inImage)->exportedSymbolAddress(&sym);
								return addr;
							}
						}
					}
				}
			}
			
			/* twolevel symbol not found */
		}
		
		sUnresolvedCount++;
		lnk::warn("[resolve, %d]: can't resolve '%s' in %s, needed by %s",
				  sUnresolvedCount,
				  symbolName,
				  ns,
				  requiredBy->getShortName());
		
		/*
			This is a hack for debugging unresolved symbols.
			Should work for counts below 4096 as we would hit PAGEZERO
			which is not executable and thus segfault.
		 */
		return (uintptr_t)sUnresolvedCount;
	}
	
	/*
		derp
	 */
	char* openWithPrefix(const char* path, const char* prefix, int* fd) {
		int sz = strlen(path) + strlen(prefix) + sizeof(char);
		char* nn = (char*)malloc(sz);
		memset((void*)nn, 0, sz);
		memcpy((void*)nn, (const void*)prefix, strlen(prefix));
		strcat(nn, path);
		
		*fd = open(nn, O_RDONLY);
		
		return nn;
	}
	
	Image* loadLibrary(const char* path) {
		bool al;
		Image* im;
		
		im = loadLibraryEx(path, &al);
		
		return im;
	}
	
	/*
		Load a library from a file and add it to the chain.
	 */
	Image* loadLibraryEx(const char* path, bool* already_loaded) {
		Image* im;
		
		if (strcmp(path, LIBSYSTEM_PATH) == 0 ||
			strcmp(path, LIBGCC_PATH) == 0) {
			/*
			 * libSystem.B.dylib doesn't exist on this OS. We either link against
			 * the linker or one of the CoreFramework sublibs which contain
			 * everything that libSystem must contain.
			 */
			return sFirstImage;
		}

		
		/*
		 * Enumerate through images and check that they are not
		 * loaded into the chain.
		 */
		im = sFirstImage;
		while (im != NULL) {
			if (im->origFilePath() != NULL &&
				strcmp(path, im->origFilePath()) == 0)
			{
				/* image is already loaded */
				*already_loaded = true;
				return im;
			}
			
			/* move on */
			im = im->nextImage();
		}
		
		*already_loaded = false;
		
		/*
			Load the new image.
		 */
		int fd;
		char* nn;
		
		nn = openWithPrefix(path, "", &fd); if (fd < 0) free(nn); else goto done;
		nn = openWithPrefix(path, "/System/", &fd); if (fd < 0) free(nn); else goto done;
		nn = openWithPrefix(path, "/usr/lib/", &fd); if (fd < 0) free(nn); else goto done;
		
	done:
		if (fd < 0) {
			/*
				Missing an image. This probably means that we're screwed.
			 */
			if (1) {
				lnk::warn("image not loaded (open=%d) {path=%s}", fd, path);
				return sDylinker; /* debug hack */
			}
			else
			{
				lnk::halt("image not loaded (open=%d) {path=%s}", fd, path);
			}
			
		}
		else {
			if (sVerboseLnkLog) {
				lnk::log("image loaded {path=%s}", nn);
			}
		}
		
		struct stat finfo;
		stat(nn, &finfo);
		
		MachObject* lib =
		MachObject::instantiateFromFile(nn, path, fd, &finfo);
		
		/* Add the new image to the front of the chain. */
		im = sFirstImage;
		((Image*)lib)->setNextImage(im->nextImage());
		im->setNextImage((Image*)lib);
		sImageCount++;
		
		/* Initialize the loaded image */
		lib->init();
		lib->doRebase();
		
		return ((Image*)lib);
	}
	
	/*
		Binds all the symbols in the image chain.
	 */
	void bindChain() {
		Image* im;
		im = sFirstImage;
		while (im != NULL) {
			im->doBindSymbols();
			
			/* move on */
			im = im->nextImage();
		}
	}
	
	/*
		Initializes all the images in the chain.
	 */
	void initializeChain() {
		Image* im;
		bool hasUnitializedDependencies;
		bool runCircularInitializer;
		int ccount = 0;
		int icount = sImageCount;
		
	run_init:
		im = sFirstImage;
		hasUnitializedDependencies = false;
		while (im != NULL) {
			if (im->dependenciesInitialized())
			{
				if (!im->hasInitialized()) 
				{
					/* All dependencies have initialized */
					
					if (sVerboseLnkLog) {
						lnk::log("initalizing image [dresolv] '%s'", im->getShortName());
					}
					im->doInitialize();
					icount--;
				}
			}
			else
			{
				/* check for circular dependencies */
				Image** deps;
				int dcount;
				bool isCirc = false;
				
				if (ccount == icount) {
					if (sVerboseLnkLog) {
						lnk::log("resolved all direct deps, running circular init ...");
					}
					goto circ_init;
				}
				
				deps = im->getDependencies(&dcount);
				
				for (int i = 0; i < dcount; i++)
				{
					if (deps[i]->dependsOn(im))
					{
						runCircularInitializer = true;
						isCirc = true;
						ccount++;
						
						if (sVerboseLnkLog) {
							lnk::log("circular %s<->%s", im->getShortName(), deps[i]->getShortName());
						}
					}
				}
				
				if (!isCirc) {
					if (sVerboseLnkLog) {
						lnk::log("image '%s' has deps, skip", im->getShortName());
					}
					hasUnitializedDependencies = true;
				}
				else 
				{
					if (sVerboseLnkLog) {
						lnk::log("image '%s' is circular (cc:%d ic:%d)", im->getShortName(), ccount, icount);
					}
				}
			}
			
			/* move on */
			im = im->nextImage();
		}
		
		if (hasUnitializedDependencies) {
			goto run_init;
		}
		
	circ_init:
		if (runCircularInitializer)
		{			
			im = sFirstImage;
			while (im != NULL) {
				if (!im->hasInitialized()) 
				{
					/*
					 * no idea what is going on so just force init it
					 * this should work on almost all libs
					 */
					if (sVerboseLnkLog) {
						lnk::log("initalizing image [circular] '%s'", im->getShortName());
					}
					im->doInitialize();
				}
				
				/* move on */
				im = im->nextImage();
			}
		}
	}
	
	void callInitializer(Initializer fn, Image* image)
	{
		//if (sVerboseLnkLog) {
			lnk::log("(%s): run static initializer @ %p", image->getShortName(), fn);
		//}
		fn(0, NULL, NULL, NULL, NULL);
	}
	
	/*
		Load auxiliary libraries.
	 
		These libraries are meant to be a part of libSystem. However, due to
		the integration issues, they are left as separate dylibs. To make sure that
		the linkage is correct, we just link these in separately. This is not a 
		disadvantage, just a slightly different way of grouping libs.
	 
		OSInitializeAuxiliaryRuntime needs these to be linked in before it runs.
	 
		Don't forget to update AUX_COUNT if changing this.
	 */
	void loadAuxiliarySystemLibraries() {
		/* Libmath, a standard part of libSystem */
		sAuxLibs[0] =
		lnk::loadLibrary("/usr/lib/system/libsystem_math.dylib");
		
		/* Blocks runtime, expected to be in libSystem */
		sAuxLibs[1] =
		lnk::loadLibrary("/usr/lib/system/libsystem_blocks.dylib");
		
		/* Unwind SjLj (SetJumpLongJump) library */
		sAuxLibs[2] =
		lnk::loadLibrary("/usr/lib/system/libunwind.dylib");
		
		/* libLaunch */
		sAuxLibs[3] =
		lnk::loadLibrary("/usr/lib/system/launch.dylib");
		
		/* key manager */
		sAuxLibs[4] =
		lnk::loadLibrary("/usr/lib/system/libkeymgr.dylib");
		
		/* CC */
		sAuxLibs[5] =
		lnk::loadLibrary("/usr/lib/system/libcommonCrypto.dylib");
	}

	void elfTest() {
		const char* aa = "1.011";
		double lol = strtod((const char*)aa, NULL);
		
		lnk::log("lol %f", lol);
		
		/*****/
		const char* mali = "/System/Library/GPUSupport/libMali.so";
		struct stat finfo;
		stat(mali, &finfo);
		int fd = open(mali, O_RDONLY);
		ElfObject* elf = ElfObject::instantiateFromFile(mali, mali, fd, &finfo);
		
		lnk::halt("ELF testing complete!");
	}
	
	extern "C" void _call_main(int argc, char **argv, void* entry);
	extern "C" void lets_get_retarded(int, const void *, size_t);
	
	/*
		Entry point for the linker.
		This is called from the linker bootstrapping code, after the
		critical bits have been initialized.
	 */
	uintptr_t
	_main(const macho_header* mainExecutableMH,
		  uintptr_t mainExecutableSlide,
		  int argc,
		  const char* argv[],
		  const char* envp[],
		  const char* apple[])
	{	
		char* lolo = (char*)0x80000000;
		lnk::log("WeirdPages: %c%c%c%c ", lolo[0], lolo[1], lolo[2], lolo[3]);
		
		lnk::log("size_t: %d", sizeof(size_t));
		
		/*
			Set up global pointers.
			XXX: I need to actually fix this.
		 */
		int i = 0;
		while (i < argc)
		{
			gArgv[i] = (char*)argv[i];
			i++;
		}
		gArgv[i] = (char*)NULL;
		gArgc = argc;
		
		/*
		 * Start the embedded runtime.
		 * Do not try to load anything before this is run.
		 */
		OSInitializeMainRuntime();
		
		OSLog("mach_task_self(): %d", mach_task_self());
		
		//elfTest();
		
		kern_return_t ret;
		mach_port_name_t pr;
		int dd = 0;
		
		ret = task_for_pid(mach_task_self(), getpid(), &pr);
		OSLog("task_for_pid(): ret: %d, port: %d", ret, pr);
		
		ret = pid_for_task(pr, &dd);
		OSLog("pid_for_task(): ret: %d, pid: %d", ret, dd);
		
		vm_address_t addr = 0x70000000;
		ret = vm_allocate(mach_task_self(), &addr, 4096, false);
		
		uint32_t* tt = (uint32_t*)addr;
		
		lnk::log("vm_allocate(): ret: %d, addr: %p, deref: %p", ret, addr, *tt);
		
		uint32_t time0;
		uint32_t time1;
		uint32_t time2;
		uint32_t time3;
		
		time0 = get_u_time(NULL);
		
		/*
		 * Set up the first image in the chain.
		 * The first image is the image of the core framework.
		 * This image contains a significant portion of the linkable libSystem code.
		 *
		 * The linker image is presented as 'libSystem'
		 */
		sDylinker =
		MachObject::instantiateFromMemory("libSystem.B.dylib", lnk::lnkHeader(), lnk::lnkSlide());
		sDylinker->init();
		sImageCount++;
		sFirstImage = (Image*)sDylinker;
		
		/*
		 * Now, load the auxiliary libraries.
		 */
		lnk::loadAuxiliarySystemLibraries();
		
		/*
		 * Set up the main executable's object.
		 * This object is NOT added to the chain.
		 */
		sMainExecutable =
		MachObject::instantiateFromMemory("MainImage", mainExecutableMH, 0);
		sMainExecutable->init();
		sMainHeader = mainExecutableMH;
		
		time1 = get_u_time(NULL);

		/*
		 * cas
		 */
		cas::get()->init();
		
		/*
		 * Bind the symbols for everything else.
		 */
		lnk::bindChain();
		
		/*
		 * Bind the symbols for the main executable.
		 */
		sMainExecutable->doBindSymbols();
		
		/*
		 * Run the initializers for the rest of libs that were just linked
		 * in by the core framework thing. This can only run after stuff has
		 * been linked in.
		 */
		OSInitializeAuxiliaryRuntime();
		
		time2 = get_u_time(NULL);
		
		/*
		 * Now run the initializers (constructors) for all loaded images.
		 */
		lnk::initializeChain();
		
		/*
		 * At last, run the initializers in the main image.
		 */
		sMainExecutable->doInitialize();
		
		/*
		 * Some test stuff is in Tester.c. Call it.
		 */
		TesterStart();
	
		time3 = get_u_time(NULL);
		
		lnk::log("time: [load: %d, bind: %d, init: %d]", time1 - time0, time2 - time1, time3 - time2);
		
		/*
		 *  Execute the main image.
		 */
		if (sVerboseLnkLog) {
			lnk::log("starting '%s' {pc=%p} ...", sMainExecutable->getShortName(), sMainExecutable->getMain());
		}
		
		/*
		 * See 'crt.s'.
		 *
		 * This function calls the entry point in the main image.
		 * It should never return. At least, I don't think so.
		 * Remind me to look into this, it might need an _exit call.
		 */
		_call_main(gArgc, gArgv, sMainExecutable->getMain());
		
		return 0;
	}
}