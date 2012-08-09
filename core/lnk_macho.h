/*
 * lnk_macho.h
 * Copyright (c) 2012 Christina Brooks
 *
 * MachO Image Loader. 
 */

#ifndef core_lnk_macho_h
#define core_lnk_macho_h

#include "macho.h"
#include "lnk.h"
#include "lnk_image.h"

class MachObject : public Image {
	
public:
	static MachObject* instantiateFromMemory(const char* moduleName, const macho_header* mh, intptr_t slide);
	static MachObject* instantiateFromFile(const char* path, const char* origPath, int fd, const struct stat* info);
	
	uint32_t* segmentCommandOffsets();
	void parseLoadCommands();
	bool findExportedSymbol(const char* symbol, Symbol* sym);
	uintptr_t exportedSymbolAddress(Symbol* sym);
	void parseDyldInfo();
	virtual BridgeEntry* getBridgeEntry();
	
	/*
		Entry point (main)
	 */
	void* getMain();
	
	/*
		Dyld info handlers.
	 */
	void readBind(const uint8_t* start, const uint8_t* end);
	void readWeakBind(const uint8_t* start, const uint8_t* end);
	void readLazyBind(const uint8_t* start, const uint8_t* end);
	void readRebase(const uint8_t* start, const uint8_t* end);
	void rebaseAt(uintptr_t addr, uintptr_t slide, uint8_t type);
	void getTextSegmentBounds(uintptr_t* start, uintptr_t* end);
	
	void processSections();
	
	void classicBindExterns();
	void classicBindIndirect();
	
	/*
		Triggers.
	 */
	void doRebase();
	void doBindSymbols();
	void doInitialize();
	void init();
	virtual ImageType getImageType();
	
	/*
	 * Classic.
	 */
	uintptr_t getClassicRelocBase();
	void classicRebase();
	
	/*
	 * Binder.
	 */
	void bind(uintptr_t address,
			  uint8_t type,
			  const char* name,
			  uint8_t flags,
			  intptr_t addend,
			  int ordinal,
			  const char* msg);
	
	/*
		Segment operations
	 */
	macho_segment_command* segLoadCmd(int index);
	uintptr_t segActualLoadAddr(int index);
	uintptr_t segActualEndAddr(int index);
	uintptr_t segFileOffset(int index);
	
	const struct mach_header* getMachHeader(); 
	uintptr_t getSlide();
	
	void throwBadBindingAddress(uintptr_t address,
								uintptr_t segmentEndAddress,
								int segmentIndex, 
								const uint8_t* startOpcodes,
								const uint8_t* endOpcodes,
								const uint8_t* pos);
	
	bool findExportedSymbolClassic(const char* symbol, Symbol* sym);
	bool findExportedSymbolCompressed(const char* symbol, Symbol* sym);
	
	uintptr_t exportedSymbolAddressClassic(Symbol* sym);
	uintptr_t exportedSymbolAddressCompressed(Symbol* sym);
	
	bool segWriteable(unsigned int segIndex);
	uintptr_t getFirstWritableSegmentAddress();
	
protected:
	/*
		An array of segment command pointers.
	 */
	macho_segment_command* fSegmentLoadCommands[64];
	uint32_t fSegmentSlides[64];
	void* fEntryPoint;
	struct routines_command* fRoutines;
	
	Image* fReExports[128];
	unsigned fReExportCount;
	
	bool fIsClassic;
	bool fIsSplitSeg;
	uintptr_t fSlide;
	const uint8_t* fLinkEditBase;
	uint32_t fSegmentsCount;
	const macho_header* fHeader;
	BridgeEntry* fBridgeEntry;
	struct dyld_info_command* fDyldInfo;
	
	/* Classic */
	const struct dysymtab_command* fDynamicInfo;
	const struct nlist* fSymtab;
	const char* fStrings;
};

#endif
