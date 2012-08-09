/*
 * lnk_elf.h
 * Copyright (c) 2012 Christina Brooks
 *
 * ELF Image Loader.
 */

#ifndef core_lnk_elf_h
#define core_lnk_elf_h

#include "lnk.h"
#include "elf_format.h"

class ElfObject : public Image {
	
public:
	static ElfObject* instantiateFromFile(const char* path, const char* origPath, int fd, const struct stat* info);
	
	/*
		Triggers.
	 */
	void doRebase();
	void doBindSymbols();
	void doInitialize();
	virtual ImageType getImageType();
	
	void parseDynamicSegment();
	void readReloc(Elf32_Rel* rel, uint32_t size);
protected:
	Elf32_Ehdr* fHeader;
	Elf32_Dyn* fDynamic;
	
	/*
		Dynamic information
		These all point to absoulute locations (slid)
	 */
	Elf32_Sym* fSymtab;
	
	Elf32_Rel* fJmpRel;
	const char* fStrtab;
	
	Elf32_Rel* fRel;
	uint32_t fRelSize;
	
	Elf32_Rel* fPltRel;
	uint32_t fPltRelSize;
	
	uintptr_t fSlide;
};

#endif
