/*
 * Core Framework: Linker: lnk_bootstrap.cpp
 * Copyright (c) 2012 Christina Brooks
 *
 * Linker initialization code. If any of this stuff fails, we're 
 * up shit creek.
 */

#include <mach/mach.h>
#include <mach-o/loader.h>
#include <mach-o/ldsyms.h>
#include <mach-o/nlist.h> 
#include <mach-o/reloc.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lnk.h"
#include "lnk_debug.h"
#include "lnk_util.h"
#include "macho.h"

extern "C" {
	/* Syscall */
	extern void* brk$LINUX(void* end);	
	extern int reboot$LINUX(int magic, int magic2, int cmd, void* arg);
	extern int __Break;
	extern void _printAbrt(void);
	extern void __sinit(void);
	
	extern const char* gLibCoreBuildDate;
	extern const char* gLibCoreVersion;
	extern const char* gLibCoreName;
	extern const char* gLibCoreVersioningUser;
}

/* pointer math */
#define addUintPtr2(x, y) (const uint8_t*)((uintptr_t)x + (uintptr_t)y)
#define addUintPtr3(x, y, z) (const uint8_t*)((uintptr_t)x + (uintptr_t)y + (uintptr_t)z)

/* update AFTER rebasing */
static const macho_header* sLinkerHeader = NULL;
static intptr_t sLinkerSlide = NULL;

/* Bootstrapper */
namespace lnk {
	
	const macho_header* lnkHeader() {
		return sLinkerHeader;
	}
	
	const intptr_t lnkSlide() {
		return sLinkerSlide;
	}
	
	namespace bootstrap {
		
		/*
		 * Called when a rebase opcode is executed.
		 */
		static void rebaseAt(uintptr_t addr, uintptr_t slide, uint8_t type)
		{	
			uintptr_t* locationToFix = (uintptr_t*)(addr);
			
			//lnk::ldbg::printNumber("rebase      @ ", *locationToFix);
			//lnk::ldbg::printNumber("rebase addr @ ", addr);
			
			switch (type) {
				case REBASE_TYPE_POINTER:
					*locationToFix += slide;
					break;
				case REBASE_TYPE_TEXT_ABSOLUTE32:
					*locationToFix += slide;
					break;
				default:
					lnk::ldbg::printText("LNK_RELOC_ERROR: baaaad rebase type\n");
			}
		}
		
		/*
		 * This is used by the sliding function to read and execute
		 * the compressed reloc table.
		 */
		static void rebaseCompressed(struct dyld_info_command* dyldInfo,
									 macho_segment_command* linkEdit,
									 uintptr_t slide,
									 macho_segment_command** segments,
									 int segCount)
		{
			/*
			 * HARD MODE!!!
			 */
			
			const uint8_t* base = (uint8_t*)((linkEdit->vmaddr + slide) - linkEdit->fileoff);
			const uint8_t* start = addUintPtr2(dyldInfo->rebase_off, base);
			const uint8_t* end = addUintPtr3(dyldInfo->rebase_size, dyldInfo->rebase_off, base);
			const uint8_t* p = start;
			
			/*
			 * If you want a better documented version of this code, see 'MachObject.cpp'
			 */
			
			uint8_t type = 0;
			int segmentIndex = 0;
			uintptr_t address = (segments[0]->vmaddr + slide);
			uintptr_t segmentEndAddress = (segments[0]->vmaddr + slide + segments[0]->vmsize);
			uint32_t count;
			uint32_t skip;
			
			bool done = false;
			while ( !done && (p < end) ) {
				uint8_t immediate = *p & REBASE_IMMEDIATE_MASK;
				uint8_t opcode = *p & REBASE_OPCODE_MASK;
				
				++p;
				
				switch (opcode) {
					case REBASE_OPCODE_DONE:
						done = true;
						break;
					case REBASE_OPCODE_SET_TYPE_IMM:
						type = immediate;
						break;
					case REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:
						segmentIndex = immediate;
						if (segmentIndex > segCount)
							lnk::ldbg::printText("LNK_RELOC_ERROR: baaaad whatever \n");
						
						address = (segments[segmentIndex]->vmaddr + slide) + read_uleb128(p, end);
						segmentEndAddress = (segments[segmentIndex]->vmaddr + segments[segmentIndex]->vmsize + slide);
						break;
					case REBASE_OPCODE_ADD_ADDR_ULEB:
						address += read_uleb128(p, end);
						break;
					case REBASE_OPCODE_ADD_ADDR_IMM_SCALED:
						address += immediate*sizeof(uintptr_t);
						break;
					case REBASE_OPCODE_DO_REBASE_IMM_TIMES:
						for (int i=0; i < immediate; ++i) {
							if ( address >= segmentEndAddress ) 
								lnk::ldbg::printText("LNK_RELOC_ERROR: baaaad REBASE_OPCODE_DO_REBASE_IMM_TIMES \n");
							rebaseAt(address, slide, type);
							address += sizeof(uintptr_t);
						}
						break;
					case REBASE_OPCODE_DO_REBASE_ULEB_TIMES:
						count = read_uleb128(p, end);
						for (uint32_t i=0; i < count; ++i) {
							if ( address >= segmentEndAddress ) 
								lnk::ldbg::printText("LNK_RELOC_ERROR: baaaad REBASE_OPCODE_DO_REBASE_ULEB_TIMES \n");
							rebaseAt(address, slide, type);
							address += sizeof(uintptr_t);
						}
						break;
					case REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB:
						if ( address >= segmentEndAddress ) 
							lnk::ldbg::printText("LNK_RELOC_ERROR: baaaad REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB \n");
						rebaseAt(address, slide, type);
						address += read_uleb128(p, end) + sizeof(uintptr_t);
						break;
					case REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB:
						count = read_uleb128(p, end);
						skip = read_uleb128(p, end);
						for (uint32_t i=0; i < count; ++i) {
							if ( address >= segmentEndAddress ) 
								lnk::ldbg::printText("LNK_RELOC_ERROR: baaaad REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB \n");
							rebaseAt(address, slide, type);
							address += skip + sizeof(uintptr_t);
						}
						break;
					default:
						_printAbrt();
				}
			}
		}
		
		/*
			Slide linker.
		 */
		static void rebaseLinker(const macho_header* mh, uintptr_t slide) 
		{
			/* load commands */
			size_t offset = sizeof(macho_header);
			uint32_t ncmds = mh->ncmds;
			uint8_t* addr = (uint8_t*)mh;
			
			/* this is to fixup linkedit */
			macho_segment_command* linkEditSeg = NULL;
			
			int segCount = 0;
			macho_segment_command* segArray[64];
			
			struct dyld_info_command* dyldInfo = NULL;
			
			while (ncmds--) {
				
				/* LC pointer */
				struct load_command	*lcp = 
				(struct load_command *)(addr + offset);
				
				offset += lcp->cmdsize;
				
				switch (lcp->cmd) {
					case LC_SEGMENT:
					{
						macho_segment_command* seg = (macho_segment_command*)lcp;
						
						segArray[segCount] = seg;
						segCount++;
						
						if (strcmp(seg->segname, SEG_LINKEDIT) == 0)
							linkEditSeg = seg;
						
						/*
						 * No need to update the lazy pointers here, they're updated
						 * by the compressed reloc table.
						 */
						break;
					}
					case LC_DYLD_INFO_ONLY:
					{
						dyldInfo = (struct dyld_info_command*)lcp;
						break;
					}
					default:
					{
						break;
					}
				}
			} /* while */
			
			/*
			 * Rebase data pointers.
			 */
			if (dyldInfo != NULL) {;
				/*
					Compressed relocation for dyld.
						* dyld info
						* linkedit
						* slide
				 */
				rebaseCompressed(dyldInfo,
								 linkEditSeg,
								 slide,
								 segArray,
								 segCount);
				
			}
			else {
				/* oops, for some reason lnk's linkedit is not compressed */
				_printAbrt();
			}
		}
		
		/* 
			check if the main text segment has been shifted
			this will always return 0x0 because the header is 
			subcontained within the text segment
		 */
		static uintptr_t slideOfMainExecutable(const macho_header* mh)
		{
			const uint32_t cmd_count = mh->ncmds;
			
			const struct load_command* const cmds = 
			(struct load_command*)(((char*)mh)+sizeof(macho_header));
			
			const struct load_command* cmd = cmds;
			for (uint32_t i = 0; i < cmd_count; ++i) {
				if ( cmd->cmd == LC_SEGMENT_COMMAND ) {
					const macho_segment_command* segCmd = (macho_segment_command*)cmd;
					if ( strcmp(segCmd->segname, "__TEXT") == 0 ) {
						return (uintptr_t)mh - segCmd->vmaddr;
					}
				}
				cmd = (const struct load_command*)(((char*)cmd)+cmd->cmdsize);
			}
			return 0;
		}
		
		/*
		 * Initialization.
		 */
		uintptr_t start(const macho_header* exec_header,
						int argc,
						const char* argv[],
						intptr_t slide, 
						const macho_header* lnk_header)
		{
			if (slide != 0) {
				
				//lnk::ldbg::printNumber("lnk: rebased @ ", slide);
				
				
				/*
				 * This will relocate the nl symbol pointers for the linker.
				 * It must be called before any complex functions are called, since
				 * globals are not relative.
				 * 
				 * This is only needed if slide != 0. If slide is zero then the
				 * linker has been compiled with a fixed load address thing.
				 */
				lnk::bootstrap::rebaseLinker(lnk_header, slide);
			}
			
			
#if (1)
			/*
			 * Print version info stuff.
			 *
			 * It's probably unnecessary to print this in production.
			 * It was really nice at first though :)
			 */
			lnk::log("%s %s (built by %s on %s) \n"
					 "            slide: %d\n"
					 "            main header: %p\n"
					 "            lnk header: %p",
					 gLibCoreName,
					 gLibCoreVersion,
					 gLibCoreVersioningUser,
					 gLibCoreBuildDate,
					 
					 slide,
					 exec_header,
					 lnk_header);
			
#endif
			
			
			sLinkerHeader = lnk_header;
			sLinkerSlide = slide;
			
			uintptr_t execSlide = slideOfMainExecutable(exec_header);
			
			lnk::_main(exec_header,
					   execSlide,
					   argc,
					   argv,
					   NULL,
					   NULL);
			
			lnk::halt("lnk::_main returned, something went wrong!");
		
			/* if something went wrong, loop so the kernel doesn't panic because of a
			 * launchd crash */
			_exit(0);
		}
		
	}
}