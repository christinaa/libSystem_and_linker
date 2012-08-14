/*
 * lnk_macho.cpp
 * Copyright (c) 2012 Christina Brooks
 *
 * MachO Image Loader. Does both classic and compressed
 * images.
 */

/*
 * XXX:
 *   > Set the correct vm_protect flags after linking.
 *   > Needs far more sanity checks.
 *   > Some of the code is sloppy.
 */

#include "lnk_macho.h"
#include "lnk.h"
#include "lnk_mm.h"
#include "lnk_util.h"

#include <mach-o/reloc.h>

#ifndef _OPEN_SOURCE
	#include "mach-o/arm/reloc.h"
#else
	#define ARM_RELOC_PB_LA_PTR 4
#endif

/* Compile-time sanity (this code only works on arm32) */
#if !defined(__arm__)
	#error This will not work on non-ARM platforms, don't even try it.
#endif

/* is that a dead dove? */
#define addUintPtr2(x, y) (const uint8_t*)((uintptr_t)x + (uintptr_t)y)
#define addUintPtr3(x, y, z) (const uint8_t*)((uintptr_t)x + (uintptr_t)y + (uintptr_t)z)
#define pageAlignPtr(x) (void*)(round_page((uint32_t)x))

static bool gVerboseLog = false;
extern "C" char*** gEnvironPtr;

/**/
extern void linker_image_table_add(uintptr_t load_addr, size_t size, const char* name);

uintptr_t MachObject::getClassicRelocBase()
{
	if (fIsSplitSeg) {
		return getFirstWritableSegmentAddress();
	}
	else {
		return segActualLoadAddr(0);
	}
}

bool MachObject::segWriteable(unsigned int segIndex)
{
	return ( (segLoadCmd(segIndex)->initprot & VM_PROT_WRITE) != 0);
}

void MachObject::getTextSegmentBounds(uintptr_t* start, uintptr_t* end)
{
	*start = segActualLoadAddr(0);
	*end = segActualEndAddr(0);
}

uintptr_t MachObject::getFirstWritableSegmentAddress()
{
	/* in split segment libraries r_address is offset from first writable segment */
	for(unsigned int i=0; i < fSegmentsCount; ++i) {
		if ( segWriteable(i) ) 
			return segActualLoadAddr(i);
	}
	lnk::halt("getFirstWritableSegmentAddress: no writable segment");
}

void MachObject::classicRebase()
{
	/*
	 * ARM only.
	 * Don't you dare running this on split seg images.
	 */
	
	register const uintptr_t slide = this->fSlide;
	const uintptr_t relocBase = this->getClassicRelocBase();
	
	if (!fDynamicInfo) {
		lnk::halt("no dsymtab section");
	}
	
	const relocation_info* const relocsStart = 
	(struct relocation_info*)(&fLinkEditBase[fDynamicInfo->locreloff]);
	
	const relocation_info* const relocsEnd = 
	&relocsStart[fDynamicInfo->nlocrel];
	
	//lnk::halt("");
	int i = 0;
	for (const relocation_info* reloc=relocsStart; reloc < relocsEnd; ++reloc) {
		i++;
		
		if ( (reloc->r_address & R_SCATTERED) == 0 ) {

			if ( reloc->r_symbolnum == R_ABS ) {
				// ignore absolute relocations
				lnk::halt("abs");
			}
			else if (reloc->r_length == RELOC_SIZE) {
				if (reloc->r_type == GENERIC_RELOC_VANILLA) {
					uintptr_t* pp = (uintptr_t*)(reloc->r_address + relocBase);
					
					*(pp) = (*pp + slide);
				}
				else {
					lnk::halt("unknown local relocation type");
				}
			}
			else {
				lnk::halt("bad local relocation length");
			}
		}
		else {
			const struct scattered_relocation_info* sreloc = (struct scattered_relocation_info*)reloc;
			if (sreloc->r_length == RELOC_SIZE) {
				uintptr_t* locationToFix = (uintptr_t*)(sreloc->r_address + fSegmentSlides[1]);

				switch(sreloc->r_type) {
					case GENERIC_RELOC_VANILLA:
						*locationToFix += slide;
						break;
					case ARM_RELOC_PB_LA_PTR:
						break;
					default:
						lnk::halt("unknown local scattered relocation type (%d)", sreloc->r_type);
				}
			}
			else {
				lnk::halt("bad local scattered relocation length");
			}
		}
	}
}

MachObject* MachObject::instantiateFromFile(const char* path, const char* origPath, int fd, const struct stat* info)
{
	macho_header header;
	uint32_t ncmds = 0;
	void* cmd_base = NULL;
	uint32_t loader_bias = 0;
	size_t offset = 0;
	uint32_t map_size = 0;
	macho_segment_command* segments[64];
	uint32_t segment_count = 0;
	void* actual_map_address = NULL;
	uint32_t file_size = 0;
	
	if (gVerboseLog) {
		lnk::log("institating a MachObject from file\n"
				 "            path  : %s\n"
				 "            fd    : %p"
				 , path, fd);
	}
	
	/* Read mach-o header */
	memset(&header, 0, sizeof(header));
	read_at(fd, &header, sizeof(header), 0);
	
	/* Check sanity */
	if (header.magic != MH_MAGIC) {
		lnk::halt("instantiateFromFile(%s): invalid magic (%p instead of %p)",
				  path,
				  header.magic,
				  MH_MAGIC);
	}
	
	/* Read load commands into a buffer */
	cmd_base = malloc(header.sizeofcmds);
	read_at(fd, cmd_base, header.sizeofcmds, sizeof(macho_header));
	
	/* Set stuff */
	ncmds = header.ncmds;
	
	/* Build array of segment load commands  */
	for (int i = 0; i < ncmds; i++)
	{
		struct load_command	*lcp = 
		(struct load_command *)((size_t)cmd_base + offset);
		
		offset += lcp->cmdsize;
		
		if (lcp->cmd == LC_SEGMENT)
		{
			segments[segment_count] = (macho_segment_command*)lcp;
			map_size += segments[segment_count]->vmsize;
			segment_count++;
		}
	}
	
	/* Check segment sanity */
	if (segment_count < 1) {
		lnk::halt("instantiateFromFile(%s): no segments", path);
	}
	
	/* Is this a fixed-load dylib? */
	if (segments[0]->vmaddr != 0) {
		/* Fixed, so not reserving */
		loader_bias = 0;
	}
	else
	{
		/* Reserve a continous range */
		loader_bias = (uint32_t)lnk::mm::reserve(map_size);
	}
	
	/* Map the segments */
	for (int i = 0; i < segment_count; i++)
	{
		macho_segment_command* seg = segments[i];
		uint32_t delta = 0;
		vm_size_t mm_filesize = round_page(seg->filesize);
		vm_size_t mm_vmsize = round_page(seg->vmsize);
		
		actual_map_address = 
		lnk::mm::wire(fd,
					  pageAlignPtr((void*)addUintPtr2(seg->vmaddr, loader_bias)),
					  mm_filesize,
					  seg->fileoff);
		
		if (gVerboseLog) {
			lnk::log("(%s:%s): wired at %p (sz: %p)", path, seg->segname, actual_map_address, mm_filesize);
		}
		
		delta = mm_vmsize - mm_filesize;
		
		if (delta && !loader_bias)
		{
			/*
			 * We need to zero-fill the memory as this space
			 * isn't backed up be reserved memory.
			 */
			
			void* anon_load = 
			(void*)(addUintPtr3(seg->vmaddr, loader_bias, mm_filesize));
			
			anon_load = pageAlignPtr(anon_load);
			
			/* map anonymous memory to fill the rest */
			actual_map_address = 
			lnk::mm::wire_anon(anon_load,
							   delta);
			
			if (gVerboseLog) {
				lnk::log("(%s:%s): wired anon at %p (sz: %p)", path, seg->segname, actual_map_address, delta);
			}
		}
	}
	
	/* Add __TEXT to the linker table */
	linker_image_table_add((uintptr_t)addUintPtr2(segments[0]->vmaddr, loader_bias),
						   (size_t)segments[0]->vmsize,
						   path);
	
	/* Instiate a new object */
	MachObject* obj = new MachObject();
	
	/* Set the object properties */
	obj->fSlide = (uintptr_t)loader_bias;
	obj->fHeader = (macho_header*)addUintPtr2(segments[0]->vmaddr, loader_bias);
	obj->fModuleName = NULL;
	obj->fFilePath = path;
	obj->fOrigFilePath = origPath;
	obj->fIsSplitSeg = (loader_bias == 0);
	
	return obj;
}

void MachObject::init()
{
	this->parseLoadCommands();
}

uintptr_t MachObject::getSlide() {
	return fSlide;
}

const struct mach_header* MachObject::getMachHeader() 
{
	return fHeader;
}

MachObject* MachObject::instantiateFromMemory(const char* moduleName, const macho_header* mh, intptr_t slide) 
{
	if (gVerboseLog) {
		lnk::log("institating a MachObject from memory\n"
				 "            name  : %s\n"
				 "            header: %p\n"
				 "            slide : %d"
				 , moduleName, mh, slide);
	}
	
	MachObject* obj = new MachObject();
	obj->fHeader = mh;
	obj->fModuleName = moduleName;
	obj->fSlide = slide;
	
	return obj;
}

void MachObject::rebaseAt(uintptr_t addr, uintptr_t slide, uint8_t type)
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
			lnk::halt("macho baaaad rebase type\n");
	}
}

macho_segment_command* MachObject::segLoadCmd(int index) 
{
	return fSegmentLoadCommands[index];
}

uintptr_t MachObject::segActualLoadAddr(int index) 
{
	return segLoadCmd(index)->vmaddr + fSlide;
}

uintptr_t MachObject::segActualEndAddr(int index) 
{
	return segActualLoadAddr(index) + segLoadCmd(index)->vmsize;
}

uintptr_t MachObject::segFileOffset(int index) 
{
	return segLoadCmd(index)->fileoff;
}

uintptr_t MachObject::exportedSymbolAddressCompressed(Symbol *sym)
{
	const uint8_t* exportNode = (uint8_t*)(sym->addr);
	const uint8_t* start = addUintPtr2(fLinkEditBase, fDyldInfo->export_off);
	const uint8_t* end = addUintPtr3(fLinkEditBase, fDyldInfo->export_off, fDyldInfo->export_size);
	bool runResolver = true;
	uintptr_t result = 0;
	
	if ((exportNode < start) || (exportNode > end))
		lnk::halt("symbol not in a trie");
	
	uint32_t flags = read_uleb128(exportNode, end);
	
	if ((flags & EXPORT_SYMBOL_FLAGS_KIND_MASK) == EXPORT_SYMBOL_FLAGS_KIND_REGULAR) {
		if ( runResolver && (flags & EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER) ) {
			lnk::halt("XXX: resolvers not implemented, fix macho loader on line %d", __LINE__);
			return result;
		}
		
		return read_uleb128(exportNode, end) + (uintptr_t)fHeader;
	}
	else if ((flags & EXPORT_SYMBOL_FLAGS_KIND_MASK) == EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL) {
		if (flags & EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER)
			lnk::halt("unsupported local exported symbol kind. flags=%d at node=%p", flags, sym);
		
		return read_uleb128(exportNode, end) + (uintptr_t)fHeader;
	}
	else {
		lnk::halt("unsupported exported symbol kind. flags=%d at node=%p", flags, sym);
	}
}

uintptr_t MachObject::exportedSymbolAddressClassic(Symbol *sym)
{
	const struct nlist* ss = (struct nlist*)(sym->addr);
	uintptr_t result = ss->n_value + fSlide;
	
	if (ss->n_desc & N_ARM_THUMB_DEF)
		result |= 1;
	
	return result;
}

uintptr_t MachObject::exportedSymbolAddress(Symbol *sym)
{
	if (fIsClassic) {
		return exportedSymbolAddressClassic(sym);
	}
	else {
		return exportedSymbolAddressCompressed(sym);
	}
}

bool MachObject::findExportedSymbolCompressed(const char* symbol, Symbol* sym)
{
	/*
	 This is a slightly tidier version of 'findExportedSymbol'
	 from dyld. Still no fucking idea what the semantics of it 
	 are since I suck at CS (lol, wtf is a trie?!).
	 */
	
	/* export table sanity */
	if (fDyldInfo->export_size == 0)
		return false;
	
	const uint8_t* start = addUintPtr2(fLinkEditBase, fDyldInfo->export_off);
	const uint8_t* end = addUintPtr3(fLinkEditBase, fDyldInfo->export_off, fDyldInfo->export_size);
	
	const uint8_t* foundNodeStart = trie_walk(start, end, symbol); 
	
	if (foundNodeStart != NULL) {
		const uint8_t* p = foundNodeStart;
		const uint32_t flags = read_uleb128(p, end);
		
		if (flags & EXPORT_SYMBOL_FLAGS_REEXPORT) {
			lnk::halt("no fucking idea, honestly");
			return false;
		}
		else {
			sym->addr = (void*)foundNodeStart;
			sym->inImage = (void*)this;
			
			return true;
		}
	}
	else {
		return false;
	}
}

bool MachObject::findExportedSymbolClassic(const char* name, Symbol* sym_)
{
	const struct nlist* sym = NULL;
	
	if (fDynamicInfo->tocoff == 0)
	{
		sym = binary_search(name,
							fStrings,
							&fSymtab[fDynamicInfo->iextdefsym],
							fDynamicInfo->nextdefsym);
	}
	else
	{
		sym = binary_search_toc(name,
								fStrings,
								fSymtab,
								(dylib_table_of_contents*)&fLinkEditBase[fDynamicInfo->tocoff], 
								fDynamicInfo->ntoc,
								fDynamicInfo->nextdefsym);
	}
	
	//printf("classic_lookup(%s): %p \n", name, sym);
	
	if (sym != NULL) {
		sym_->inImage = (void*)this;
		sym_->addr = (void*)sym;
		
		return true;
	}
	
	return false;
}

bool MachObject::findExportedSymbol(const char* symbol, Symbol* sym)
{
	bool ret = false;
	
	if (fIsClassic) {
		ret = findExportedSymbolClassic(symbol, sym);
	}
	else {
		ret = findExportedSymbolCompressed(symbol, sym);
	}
	
	if (!ret) {
		if (fReExportCount) {
			/* The symbol could be in a re-exported lib */
			
			for (int i = 0; i < fReExportCount; i++)
			{
				Symbol rexp;
				bool ret;
				
				ret = 
				fReExports[i]->findExportedSymbol(symbol, &rexp);
				
				if (ret) {
					sym->addr = rexp.addr;
					sym->inImage = rexp.inImage;
					
					return true;
				}
			}
		}
		
		/* not found anywhere! */
		return false;
	}
	else {
		/* symbol found! */
		return true;
	}	
}

void MachObject::bind(uintptr_t address,
					  uint8_t type,
					  const char* name,
					  uint8_t flags,
					  intptr_t addend,
					  int ordinal,
					  const char* msg)
{
	Image* targetImage = NULL;
	int reqOrdinal = ordinal-1;
	
	/*
	 * Perform a two-level resoultion.
	 * Ordinals start at 1, everything below is special.
	 */
	if (reqOrdinal > fDepCount) {
		lnk::halt("%s: ordinal for symbol '%s' out of range", getShortName(), name);
	}
	if (ordinal > 0) {
		/* Normal library ordinal */
		targetImage = fDependencies[reqOrdinal];
	}
	
	uintptr_t value = 
	lnk::resolve(name,
				 flags,
				 ordinal,
				 &targetImage,
				 this);

	
	/* let's rock */
	if (0) {
		if ( addend != 0 ) {
			lnk::log("link: %sbind: %s:0x%08lX = %s:%s, *0x%08lX = 0x%08lX + %ld",
					 msg, this->getShortName(), (uintptr_t)address,
					  ((targetImage != NULL) ? targetImage->getShortName() : "<weak_import-missing>"),
					  lnk::safe_symbol_name((char*)name), (uintptr_t)address, value, addend);
		}
		else {
			lnk::log("link: %sbind: %s:0x%08lX = %s:%s, *0x%08lX = 0x%08lX",
					  msg, this->getShortName(), (uintptr_t)address,
					  ((targetImage != NULL) ? targetImage->getShortName() : "<weak>import-missing>"),
					  lnk::safe_symbol_name((char*)name), (uintptr_t)address, value);
		}
	}
	
	/* update */
	uintptr_t* locationToFix = (uintptr_t*)address;
	uint32_t* loc32;
	uintptr_t newValue = value+addend;
	uint32_t value32;
	switch (type) {
		case BIND_TYPE_POINTER:
			/* do we actually need this? */
			if ( *locationToFix != newValue )
				*locationToFix = newValue;
			break;
		case BIND_TYPE_TEXT_ABSOLUTE32:
			loc32 = (uint32_t*)locationToFix;
			value32 = (uint32_t)newValue;
			if ( *loc32 != value32 )
				*loc32 = value32;
			break;
		case BIND_TYPE_TEXT_PCREL32:
			loc32 = (uint32_t*)locationToFix;
			value32 = (uint32_t)newValue - (((uintptr_t)locationToFix) + 4);
			if ( *loc32 != value32 )
				*loc32 = value32;
			break;
		default:
			lnk::halt("bad bind type %d", type);
	}
}

void MachObject::processSections()
{
	for (int i =0; i < fSegmentsCount; i++) {
		macho_segment_command* seg = segLoadCmd(i);
		
		/* For enumerating the sections */
		const macho_section* const sectionsStart =
		(macho_section*)((char*)seg + sizeof(macho_segment_command));
		
		const macho_section* const sectionsEnd = 
		&sectionsStart[seg->nsects];
		
		for (const macho_section* sect=sectionsStart; sect < sectionsEnd; ++sect) {
			uint8_t type = sect->flags & SECTION_TYPE;
			type = type; /* shut up gcc, I don't care */
			
			if (strcmp(sect->sectname, "__program_vars") == 0) {
				struct ProgramVars* pv = (struct ProgramVars*)(sect->addr + fSlide);
				
				lnk::log("'%s': program vars: {mh=%p, v=%p, c=%p, env=%p}",
						 getShortName(),
						 pv->mh,
						 pv->NXArgvPtr,
						 pv->NXArgcPtr,
						 pv->environPtr);
				
				
				/*
				 * I like the brackets around '*pv->environPtr'.
				 * Do not remove them, or it will make me sad.
				 */
				(*pv->environPtr) = (const char**)gEnvironPtr; 
			}
			else if (strcmp(sect->sectname, "__bridge") == 0) {
				BridgeEntry* br = (BridgeEntry*)(sect->addr + fSlide);
				lnk::log("'%s': bridge type: %d", getShortName(), br->mode);
				//fBridgeEntry = br;
			}
		}
	}
}

BridgeEntry* MachObject::getBridgeEntry()
{
	return fBridgeEntry;
}

void MachObject::doInitialize()
{
	/*
		Run all sorts of library initializers.
	 */
	if (fHasInitialized) {
		return;
	}
	
	if (fRoutines != NULL) {
		Initializer func = (Initializer)(fRoutines->init_address + fSlide);
		lnk::log("(%s): LC_ROUTINES initializer @ %p", getShortName(), func);
		lnk::callInitializer(func, this);
	}
	
	for (int i =0; i < fSegmentsCount; i++) {
		macho_segment_command* seg = fSegmentLoadCommands[i];
		
		/* For enumerating the sections */
		const macho_section* const sectionsStart =
		(macho_section*)((char*)seg + sizeof(macho_segment_command));
		
		const macho_section* const sectionsEnd = 
		&sectionsStart[seg->nsects];
		
		for (const macho_section* sect=sectionsStart; sect < sectionsEnd; ++sect) {
			const uint8_t type = sect->flags & SECTION_TYPE;
			
			if (gVerboseLog) {
				lnk::log("(%s): %s", getShortName(), sect->sectname);
			}
			
			if (type == S_MOD_INIT_FUNC_POINTERS) {
				/*
					This is how dyld does it.
				 */
				Initializer* inits = (Initializer*)(sect->addr + fSlide);
				const uint32_t count = sect->size / sizeof(uintptr_t);
				
				for (uint32_t i=0; i < count; ++i) {
					Initializer func = inits[i];
					
					lnk::callInitializer(func, this);
				}
			}
		}
	}
	
	fHasInitialized = true;
}

void MachObject::throwBadBindingAddress(uintptr_t address,
										uintptr_t segmentEndAddress,
										int segmentIndex, 
										const uint8_t* startOpcodes,
										const uint8_t* endOpcodes,
										const uint8_t* pos)
{
	lnk::halt("malformed binding opcodes (%ld/%ld): address 0x%08lX is beyond end of segment %d (0x%08lX -> 0x%08lX)",
				 (intptr_t)(pos-startOpcodes), (intptr_t)(endOpcodes-startOpcodes), address, segmentIndex, 
				 segActualLoadAddr(segmentIndex), segmentEndAddress); 
}

void MachObject::readLazyBind(const uint8_t* start, const uint8_t* end) 
{
	const uint8_t* pos = start;
	
	if (gVerboseLog) {
		lnk::log("binding lazy symbols\n            start %p, end %p", start, end);
	}
	
	/* Data retrieved from the opcodes */
	int libraryOrdinal = 0;
	int segmentIndex = 0;
	const char* symbolName = NULL;
	uint8_t symbolFlags = 0;
	intptr_t addend = 0;
	uint8_t type = BIND_TYPE_POINTER; /* ptr */
	
	/* XXX: get from segdata */
	uintptr_t segmentEndAddress = 0;
	uintptr_t address = 0;
	
	/* Parse opcodes */
	while (pos < end) {
		uint8_t opcode = *(pos) & BIND_OPCODE_MASK;
		uint8_t immediate = *(pos) & BIND_IMMEDIATE_MASK;
		
		pos++;
		
		switch (opcode) {
			case BIND_OPCODE_DONE:
			{	
				break;
			}
			case BIND_OPCODE_SET_DYLIB_ORDINAL_IMM:
			{	
				libraryOrdinal = immediate;
				break;
			}
			case BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB:
			{	
				libraryOrdinal = (int)read_uleb128(pos, end);
				break;
			}
			case BIND_OPCODE_SET_DYLIB_SPECIAL_IMM:
			{
				/* the special ordinals are negative numbers */
				if ( immediate == 0 )
					libraryOrdinal = 0;
				else {
					int8_t signExtended = BIND_OPCODE_MASK | immediate;
					libraryOrdinal = signExtended;
				}
				break;
			}
			case BIND_OPCODE_SET_TYPE_IMM:
			{
				type = immediate;
				break;
			}
			case BIND_OPCODE_SET_ADDEND_SLEB:
			{
				addend = read_sleb128(pos, end);
				break;
			}
			case BIND_OPCODE_ADD_ADDR_ULEB:
			{
				address += read_uleb128(pos, end);
				break;
			}
			case BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:
			{
				segmentIndex = immediate;
				
				address = segActualLoadAddr(segmentIndex) + read_uleb128(pos, end);
				segmentEndAddress = segActualEndAddr(segmentIndex);
				break;
			}
			case BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM:
			{
				symbolName = (char*)pos;
				symbolFlags = immediate;
				while (*pos != '\0')
					++pos;
				++pos;
				
				break;
			}
			case BIND_OPCODE_DO_BIND:
			{
				if ( address >= segmentEndAddress ) 
					throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, pos);
				
				this->bind(address,
						   type,
						   symbolName,
						   symbolFlags,
						   addend,
						   libraryOrdinal,
						   "lazy");
				
				address += sizeof(intptr_t);
				break;
			}
			case BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:
			case BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED:
			case BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB:
			default:
			{
				lnk::halt("invalid opcode %d for bind lazy", *pos);
				break;
			}
		}
	}
}

void MachObject::readRebase(const uint8_t* start, const uint8_t* end)
{
	/* aaaa!!!! */;
	
	uint8_t type = 0;
	int segmentIndex = 0;
	uintptr_t address = segActualLoadAddr(0);
	uintptr_t segmentEndAddress = segActualEndAddr(0);
	uint32_t count;
	uint32_t skip;
	const uint8_t* p = start;
	
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
				if (segmentIndex > fSegmentsCount)
					lnk::halt("baaaad seg count for rebase!");
				
				address = segActualLoadAddr(segmentIndex) + read_uleb128(p, end);
				segmentEndAddress = segActualEndAddr(segmentIndex);
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
						throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, p);
					this->rebaseAt(address, fSlide, type);
					address += sizeof(uintptr_t);
				}
				break;
			case REBASE_OPCODE_DO_REBASE_ULEB_TIMES:
				count = read_uleb128(p, end);
				for (uint32_t i=0; i < count; ++i) {
					if ( address >= segmentEndAddress ) 
						throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, p);
					this->rebaseAt(address, fSlide, type);
					address += sizeof(uintptr_t);
				}
				break;
			case REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB:
				if ( address >= segmentEndAddress ) 
					throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, p);
				this->rebaseAt(address, fSlide, type);
				address += read_uleb128(p, end) + sizeof(uintptr_t);
				break;
			case REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB:
				count = read_uleb128(p, end);
				skip = read_uleb128(p, end);
				for (uint32_t i=0; i < count; ++i) {
					if ( address >= segmentEndAddress ) 
						throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, p);
					rebaseAt(address, fSlide, type);
					address += skip + sizeof(uintptr_t);
				}
				break;
			default:
				lnk::halt("invalid opcode %d for rebase", *p);
				break;
		}
	}

}

void MachObject::readWeakBind(const uint8_t* start, const uint8_t* end) 
{
	const uint8_t* pos = start;
	
	if (gVerboseLog) {
		lnk::log("binding weak symbols\n            start %p, end %p", start, end);
	}
	
	/* Data retrieved from the opcodes */
	int libraryOrdinal = 0;
	int segmentIndex = 0;
	const char* symbolName = NULL;
	uint8_t symbolFlags = 0;
	intptr_t addend = 0;
	uint8_t type = 0;
	
	/* XXX: get from segdata */
	uintptr_t segmentEndAddress = 0;
	uintptr_t address = 0;
	
	uint32_t count;
	uint32_t skip;
	
	/* Parse opcodes */
	while (pos < end) {
		uint8_t opcode = *(pos) & BIND_OPCODE_MASK;
		uint8_t immediate = *(pos) & BIND_IMMEDIATE_MASK;
		
		pos++;
		
		switch (opcode) {
			case BIND_OPCODE_DONE:
			{	
				break;
			}
			case BIND_OPCODE_SET_DYLIB_ORDINAL_IMM:
			{	
				libraryOrdinal = immediate;
				break;
			}
			case BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB:
			{	
				libraryOrdinal = (int)read_uleb128(pos, end);
				break;
			}
			case BIND_OPCODE_SET_TYPE_IMM:
			{
				type = immediate;
				break;
			}
			case BIND_OPCODE_SET_ADDEND_SLEB:
			{
				addend = read_sleb128(pos, end);
				break;
			}
			case BIND_OPCODE_ADD_ADDR_ULEB:
			{
				address += read_uleb128(pos, end);
				break;
			}
			case BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:
			{
				segmentIndex = immediate;
				
				address = segActualLoadAddr(segmentIndex) + read_uleb128(pos, end);
				segmentEndAddress = segActualEndAddr(segmentIndex); 
				break;
			}
			case BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM:
			{
				symbolName = (char*)pos;
				symbolFlags = immediate;
				while (*pos != '\0')
					++pos;
				++pos;
				
				break;
			}
			case BIND_OPCODE_DO_BIND:
			{
				if ( address >= segmentEndAddress ) 
					throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, pos);
				
				/*
				this->bind(address,
						   type,
						   symbolName,
						   symbolFlags,
						   addend,
						   libraryOrdinal,
						   "weak");
				*/
				
				address += sizeof(intptr_t);
				break;
			}
			case BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:
			{
				if ( address >= segmentEndAddress ) 
					throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, pos);
				
				/*
				this->bind(address,
						   type,
						   symbolName,
						   symbolFlags,
						   addend,
						   libraryOrdinal,
						   "weak");
				*/
				
				address += read_uleb128(pos, end) + sizeof(intptr_t);
				break;
			}
			case BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED:
			{
				if ( address >= segmentEndAddress ) 
					throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, pos);
				/*
				this->bind(address,
						   type,
						   symbolName,
						   symbolFlags,
						   addend,
						   libraryOrdinal,
						   "weak");
				*/
				
				address += immediate*sizeof(intptr_t) + sizeof(intptr_t);
				break;
			}
			case BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB:
			{
				count = read_uleb128(pos, end);
				skip = read_uleb128(pos, end);
				for (uint32_t i=0; i < count; ++i) {
					if ( address >= segmentEndAddress ) 
						throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, pos);
					/*
					this->bind(address,
							   type,
							   symbolName,
							   symbolFlags,
							   addend,
							   libraryOrdinal,
							   "weak");
					*/
					
					address += skip + sizeof(intptr_t);
				}
				break;
			}
			default:
			{
				lnk::halt("invalid opcode %d for bind nl", *pos);
				break;
			}
		}
	}
}

void MachObject::readBind(const uint8_t* start, const uint8_t* end) 
{
	const uint8_t* pos = start;
	
	if (gVerboseLog) {
		lnk::log("binding non-lazy symbols\n            start %p, end %p", start, end);
	}
	
	/* Data retrieved from the opcodes */
	int libraryOrdinal = 0;
	int segmentIndex = 0;
	const char* symbolName = NULL;
	uint8_t symbolFlags = 0;
	intptr_t addend = 0;
	uint8_t type = 0;
	
	/* XXX: get from segdata */
	uintptr_t segmentEndAddress = 0;
	uintptr_t address = 0;
	
	uint32_t count;
	uint32_t skip;
	bool done = false;
	
	/* Parse opcodes */
	while (!done && pos < end) {
		uint8_t opcode = *(pos) & BIND_OPCODE_MASK;
		uint8_t immediate = *(pos) & BIND_IMMEDIATE_MASK;
		
		pos++;
		
		switch (opcode) {
			case BIND_OPCODE_DONE:
			{	
				done = true;
				break;
			}
			case BIND_OPCODE_SET_DYLIB_ORDINAL_IMM:
			{	
				libraryOrdinal = immediate;
				break;
			}
			case BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB:
			{	
				libraryOrdinal = (int)read_uleb128(pos, end);
				break;
			}
			case BIND_OPCODE_SET_TYPE_IMM:
			{
				type = immediate;
				break;
			}
			case BIND_OPCODE_SET_DYLIB_SPECIAL_IMM:
			{
				/* the special ordinals are negative numbers */
				if ( immediate == 0 )
					libraryOrdinal = 0;
				else {
					int8_t signExtended = BIND_OPCODE_MASK | immediate;
					libraryOrdinal = signExtended;
				}
				break;
			}
			case BIND_OPCODE_SET_ADDEND_SLEB:
			{
				addend = read_sleb128(pos, end);
				break;
			}
			case BIND_OPCODE_ADD_ADDR_ULEB:
			{
				address += read_uleb128(pos, end);
				break;
			}
			case BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:
			{
				segmentIndex = immediate;
				
				address = segActualLoadAddr(segmentIndex) + read_uleb128(pos, end);
				segmentEndAddress = segActualEndAddr(segmentIndex); 
				break;
			}
			case BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM:
			{
				symbolName = (char*)pos;
				symbolFlags = immediate;
				while (*pos != '\0')
					++pos;
				++pos;
				
				break;
			}
			case BIND_OPCODE_DO_BIND:
			{
				if ( address >= segmentEndAddress ) 
					throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, pos);
				
				this->bind(address,
						   type,
						   symbolName,
						   symbolFlags,
						   addend,
						   libraryOrdinal,
						   "");
				
				address += sizeof(intptr_t);
				break;
			}
			case BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:
			{
				if ( address >= segmentEndAddress ) 
					throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, pos);
				
				this->bind(address,
						   type,
						   symbolName,
						   symbolFlags,
						   addend,
						   libraryOrdinal,
						   "");
				
				
				address += read_uleb128(pos, end) + sizeof(intptr_t);
				break;
			}
			case BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED:
			{
				if ( address >= segmentEndAddress ) 
					throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, pos);
				
				this->bind(address,
						   type,
						   symbolName,
						   symbolFlags,
						   addend,
						   libraryOrdinal,
						   "");
				
				
				address += immediate*sizeof(intptr_t) + sizeof(intptr_t);
				break;
			}
			case BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB:
			{
				count = read_uleb128(pos, end);
				skip = read_uleb128(pos, end);
				for (uint32_t i=0; i < count; ++i) {
					if ( address >= segmentEndAddress ) 
						throwBadBindingAddress(address, segmentEndAddress, segmentIndex, start, end, pos);
					
					this->bind(address,
							   type,
							   symbolName,
							   symbolFlags,
							   addend,
							   libraryOrdinal,
							   "");
					
					address += skip + sizeof(intptr_t);
				}
				break;
			}
			default:
			{
				lnk::halt("invalid opcode %d for bind nl", *pos);
				break;
			}
		}
	}
}

void MachObject::classicBindIndirect()
{
	const uint32_t cmd_count = fHeader->ncmds;
	const struct load_command* const cmds = (struct load_command*)(fHeader+1);
	const struct load_command* cmd = cmds;
	const uint32_t* const indirectTable = (uint32_t*)&fLinkEditBase[fDynamicInfo->indirectsymoff];
	bool bindNonLazys = true;
	bool bindLazys = true;

	for (uint32_t i = 0; i < cmd_count; ++i)
	{
		if (cmd->cmd == LC_SEGMENT_COMMAND) {
			const macho_segment_command* seg =
			(macho_segment_command*)cmd;
			
			const macho_section* const sectionsStart =
			(macho_section*)((char*)seg + sizeof(macho_segment_command));
			
			const  macho_section* const sectionsEnd =
			&sectionsStart[seg->nsects];
			
			for (const macho_section* sect=sectionsStart; sect < sectionsEnd; ++sect)
			{
				bool isLazySymbol = false;
				const uint8_t type = sect->flags & SECTION_TYPE;
				uint32_t elementSize = sizeof(uintptr_t);
				uint32_t elementCount = sect->size / elementSize;
				
				if ( type == S_NON_LAZY_SYMBOL_POINTERS ) {
					if ( ! bindNonLazys )
						continue;
				}
				else if ( type == S_LAZY_SYMBOL_POINTERS ) {
					// process each symbol pointer in this section
					isLazySymbol = true;
					if ( ! bindLazys )
						continue;
				}
				else {
					continue;
				}
				
				const uint32_t indirectTableOffset = sect->reserved1;
				uint8_t* ptrToBind = (uint8_t*)(sect->addr + fSlide);
				for (uint32_t j=0; j < elementCount; ++j, ptrToBind += elementSize) {
					uint32_t symbolIndex = indirectTable[indirectTableOffset + j];
					if ( symbolIndex == INDIRECT_SYMBOL_LOCAL) {
						
						/* LOCA PEOPLE! */
						//lnk::log("classicBindIndirect: [local] *%p = %p", ptrToBind, *ptrToBind);
						
						*((uintptr_t*)ptrToBind) += this->fSlide;
					}
					else if ( symbolIndex == INDIRECT_SYMBOL_ABS) {
						// do nothing since already has absolute address
						lnk::halt("ABS!");
					}
					else {
						const struct nlist* sym = &fSymtab[symbolIndex];
						if ( symbolIndex == 0 ) {
							lnk::halt("malformed classic image!");
						}
						
						/* actually do stuff */
						const char* symbolName = &fStrings[sym->n_un.n_strx];
						uint8_t symbolFlags = 0; /* currently unused */
						uint8_t ord = GET_LIBRARY_ORDINAL(sym->n_desc);
						
						//lnk::log("classicBindIndirect: %s, ord: %d:(%s)", symbolName, ord, fDependencies[ord-1]->getShortName());
						
						bind((uintptr_t)ptrToBind,
							 BIND_TYPE_POINTER,
							 symbolName,
							 symbolFlags,
							 0,
							 ord,
							 "indirect");
					}
				}
			}
		}

		cmd = (const struct load_command*)(((char*)cmd) + cmd->cmdsize);
	}
}

void MachObject::classicBindExterns()
{
	const uintptr_t relocBase = getClassicRelocBase();
	
	const relocation_info* const relocsStart =
	(struct relocation_info*)(&fLinkEditBase[fDynamicInfo->extreloff]);
	
	const relocation_info* const relocsEnd =
	&relocsStart[fDynamicInfo->nextrel];
	
	for (const relocation_info* reloc=relocsStart; reloc < relocsEnd; ++reloc)
	{
		if (reloc->r_length == RELOC_SIZE) {
			if (reloc->r_type == POINTER_RELOC)
			{
				const struct nlist* undefinedSymbol = &fSymtab[reloc->r_symbolnum];
				uintptr_t* location = ((uintptr_t*)(reloc->r_address + relocBase));
				uintptr_t value = *location;

				if ( ((undefinedSymbol->n_type & N_TYPE) == N_SECT) && ((undefinedSymbol->n_desc & N_ARM_THUMB_DEF) != 0) ) {
					value -= (undefinedSymbol->n_value+1);
					
					lnk::halt("weird arm thumb thing detected!");
				}
				else {
					/* is undefined or non-weak symbol, so do subtraction to get addend */
					value -= undefinedSymbol->n_value;
				}
				
				const char* symbolName = &fStrings[undefinedSymbol->n_un.n_strx];
				uint8_t symbolFlags = 0; /* currently unused */
				uint8_t ord = GET_LIBRARY_ORDINAL(undefinedSymbol->n_desc);
				
				//lnk::log("classicBindExtern: %s, ord: %d:(%s)", symbolName, ord, fDependencies[ord-1]->getShortName());
				
				/* bind */
				bind((uintptr_t)location,
					 BIND_TYPE_POINTER,
					 symbolName,
					 symbolFlags,
					 value,
					 ord,
					 "extern");
			}
			else
			{
				lnk::halt("unknown reloc type!");
			}
		}
		else
		{
			lnk::halt("bad reloc size!");
		}
	}
}

void MachObject::doBindSymbols()
{
	/*
	 * Parsing the compressed dyld info section.
	 */
	if (fIsClassic)
	{
		this->classicBindIndirect();
		this->classicBindExterns();
	}
	else
	{
		if (!fDyldInfo) {
			lnk::halt("no dyld info section");
		}
		
		/* Bind opcodes */
		this->readBind(addUintPtr2(fLinkEditBase, fDyldInfo->bind_off),
					   addUintPtr3(fLinkEditBase, fDyldInfo->bind_off, fDyldInfo->bind_size));
		
		this->readWeakBind(addUintPtr2(fLinkEditBase, fDyldInfo->weak_bind_off),
						   addUintPtr3(fLinkEditBase, fDyldInfo->weak_bind_off, fDyldInfo->weak_bind_size));
		
		this->readLazyBind(addUintPtr2(fLinkEditBase, fDyldInfo->lazy_bind_off),
						   addUintPtr3(fLinkEditBase, fDyldInfo->lazy_bind_off, fDyldInfo->lazy_bind_size));
	}
}

void MachObject::doRebase() 
{
	if (fIsClassic)
	{
		if (!fIsSplitSeg) {
			/*
			 * Split segs do not need rebasing.
			 */
			classicRebase();
		}
	}
	else
	{
		this->readRebase(addUintPtr2(fLinkEditBase, fDyldInfo->rebase_off),
						 addUintPtr3(fLinkEditBase, fDyldInfo->rebase_off, fDyldInfo->rebase_size));
	}
}

void* MachObject::getMain()
{
	return fEntryPoint;
}

ImageType MachObject::getImageType()
{
	return kImageTypeMachO;
}

void MachObject::parseLoadCommands()
{
	const macho_header* mh = fHeader;
	
	/* load commands */
	size_t offset = sizeof(macho_header);
	uint32_t ncmds = 0;
	uint32_t segCount = 0; /* count segments */
	uint8_t* addr = (uint8_t*)mh;
	bool compressed;
	
	/* symtab */
	const struct nlist* symbolTable = NULL;
	const char* symbolTableStrings = NULL;
	
	ncmds = mh->ncmds;
	
	if (ncmds == 0) {
		lnk::halt("segmentless macho file, probably a memory goof");
	}
	
	while (ncmds--) {
		/* LC pointer */
		struct load_command	*lcp = 
		(struct load_command *)(addr + offset);
		
		//lnk::log("loading load command @ %p", addr + offset);

		offset += lcp->cmdsize;
		
		switch (lcp->cmd) {
			case LC_SEGMENT:
			{
				fSegmentLoadCommands[segCount] = (macho_segment_command*)lcp;
				macho_segment_command* seg = (macho_segment_command*)lcp;
				
				if (gVerboseLog) {
					lnk::log("processed segment {size=%d, name=%s}", lcp->cmdsize, seg->segname);
				}
				
				/* update global bits pointing to different things */
				if ((strcmp(seg->segname, SEG_LINKEDIT) == 0)) {
					fLinkEditBase = (uint8_t*)(segActualLoadAddr(segCount) - segFileOffset(segCount));
				}
				
				segCount ++;
				break;
			}
			case LC_UNIXTHREAD:
			{
				arm_thread_command* tcp = (arm_thread_command*)lcp;
				fEntryPoint = (void*)(fSlide + tcp->state.r15);
				break;
			}
			case LC_ROUTINES:
			{
				lnk::warn("LC_ROUTINES in %s", this->getShortName());
				fRoutines = (struct routines_command*)lcp;
				break;
			}
			case LC_LOAD_DYLIB:
			case LC_LOAD_WEAK_DYLIB:
			case LC_REEXPORT_DYLIB:
			case LC_LOAD_UPWARD_DYLIB:
			{
				const struct dylib_command* dylib = (struct dylib_command*)lcp;
				const char* nn = (char*)lcp + dylib->dylib.name.offset;
				
				/* XXX: sanity check on the string */
				
				
				/*
				 * Load the library that we depend on and add it to
				 * the list of dependencies for this image.
				 */
				Image* dep = lnk::loadLibrary(nn);
				if (dep) {
					fDependencies[fDepCount] = dep;
					fDepCount++;
				}
				else {
					/* should get caught out by loadLibrary, but this is 
					 * just in case */
					lnk::halt("DEP_INIT: %s failed to load", nn);
				}
				
				if (lcp->cmd == LC_REEXPORT_DYLIB) 
				{
					fReExports[fReExportCount] = dep;
					fReExportCount++;
				}
				
				break;
			}
			case LC_SYMTAB:
			{
				if (fLinkEditBase == NULL) {
					lnk::halt("LC_SYMTAB before linkedit segment!");
				}
				
				const struct symtab_command* symtab = (struct symtab_command*)lcp;
				symbolTableStrings = (const char*)&fLinkEditBase[symtab->stroff];
				symbolTable = (struct nlist*)(&fLinkEditBase[symtab->symoff]);
			}
			case LC_DYSYMTAB:
			{
				fDynamicInfo = (struct dysymtab_command*)lcp;
				break;
			}
			case LC_DYLD_INFO:
			case LC_DYLD_INFO_ONLY:
		 	{
				compressed = true;
				fDyldInfo = (struct dyld_info_command*)lcp;
				break;
			}
			default:
			{
				break;
			}
		}
	}
	
	fSegmentsCount = segCount;
	
	/* Classic */
	if (!compressed) {
		/* Classic flag used everywhere by the loader */
		fIsClassic = true;
		
		/* these are needed by the classic linker */
		fStrings = symbolTableStrings;
		fSymtab = symbolTable;
		
		lnk::log("Using a classic loader for '%s'", getShortName());
	}
	
	this->processSections();
}
