/*
 * lnk_elf.cpp
 * Copyright (c) 2012 Christina Brooks
 *
 * Elf32 Image Loader.
 * "It's ELF, it sucks!"
 */

#include "lnk_elf.h"
#include "lnk.h"
#include "lnk_mm.h"
#include "lnk_util.h"

/* this stuff will only work on arm32 due to sloppy pointer maths */
#if !defined(__arm__)
	#error can i haz arm?
#endif

/* pointer math */
#define addUintPtr2(x, y) (const uint8_t*)((uintptr_t)x + (uintptr_t)y)
#define addUintPtr3(x, y, z) (const uint8_t*)((uintptr_t)x + (uintptr_t)y + (uintptr_t)z)

void ElfObject::readReloc(Elf32_Rel* rel, uint32_t size)
{
	/*
		Read and execute the relocation entries at 'rel'.
		These contain all sorts of things including rebase info and 
		symbol information, unlike in macho files where they're distinct.
	 */
	
	Elf32_Sym* symtab = fSymtab;
	const char* strtab = fStrtab;
	uint32_t count = size / sizeof(Elf32_Rel);
	
	/* current symbol info */
	Elf32_Sym* cs_entry; 
	char* cs_name;
	uint32_t cs_addr;
	
	lnk::log("%s: processing reloc {ptr=%p, sz=%d, c=%d}", getShortName(), rel, size, count);
	
	while (count--) {
		uint32_t type = ELF32_R_TYPE(rel->r_info);
		uint32_t sym = ELF32_R_SYM(rel->r_info);
		
		/* is it a symbol? */
		if (sym != 0)
		{
			/* this is a symbol reloc */
			cs_name = (char*)(strtab + symtab[sym].st_name);
			lnk::log("sym: %s", cs_name);
			
			/* for the sake of error checking */
			switch (type) 
			{
				case R_ARM_JUMP_SLOT:
				case R_ARM_GLOB_DAT:
				case R_ARM_ABS32:
				case R_ARM_RELATIVE:
				case R_ARM_NONE:
				{
					/* Ignored */
					break;
				}
				case R_ARM_COPY:
				default:
				{
					lnk::halt("invalid ELF reloc type %d!", type);
					break;
				}	
			}
		}
		else
		{
			/* not a symbol */
			cs_entry = NULL;
		}
		
		/* now deal with the actual reloc */
		switch (type) 
		{
			default:
			{
				lnk::halt("invalid ELF reloc type %d!", type);
				break;
			}	
		}
		
		/* move on */
		rel++;
	}
}

void ElfObject::doBindSymbols()
{
	
}

ImageType ElfObject::getImageType()
{
	return kImageTypeElf;
}


void ElfObject::doInitialize()
{
	
}

void ElfObject::doRebase()
{
	/*
		Do nothing.
	 
		ELF rebase process is done as a part of the
		binding process.
	 */
}

void ElfObject::parseDynamicSegment() {
	/*
		Dynamic whatever command.
		It's an array of Elf_Dyns ended by a NULL tag.
	 */
	Elf32_Dyn* dyn = fDynamic;
	
	if (dyn == NULL) {
		lnk::halt("%s: missing dynamic segment", getShortName());
	}
	
	while ((dyn->d_tag) != 0) {
		switch (dyn->d_tag) {
				
			/* REL */
			case DT_RELSZ:
			{
				fRelSize = (uint32_t)dyn->d_un.d_val;
				break;
			}
			case DT_REL:
			{
				fRel = (Elf32_Rel*)addUintPtr2(fHeader, dyn->d_un.d_val);
				break;
			}
				
			/* PLT REL */
			case DT_PLTRELSZ:
			{
				fPltRelSize = (uint32_t)dyn->d_un.d_val;
				break;
			}
			case DT_JMPREL:
			{
				/*
					Well, logically this should be called DT_PLTREL.
					Instead, it's called DT_JMPREL. I don't know why, but
					that's what the ELF people decided.
				 
					As far as I understand, this section stores the imports.
				 */
				fPltRel = (Elf32_Rel*)addUintPtr2(fHeader, dyn->d_un.d_val);
				break;
			}
			case DT_PLTREL:
			{
				/* On the other hand, this sect is ignored :/ */
				break;
			}
			
			/* Other ones */
			case DT_RELA:
			{
				lnk::halt("DT_RELA not supported!");
				break;
			}
			case DT_STRTAB:
			{
				/* I hope this always comes before DT_NEEDED */
				fStrtab = (const char*)addUintPtr2(fHeader, dyn->d_un.d_val);
				break;
			}
			case DT_NEEDED:
			{
				if (!fStrtab) {
					lnk::halt("DT_NEEDED before DT_STRTAB, fixme!");
				}
				lnk::log("%s requires %s", getShortName(), addUintPtr2(fStrtab, dyn->d_un.d_val));
				break;
			}
			case DT_SYMTAB:
			{
				fSymtab = (Elf32_Sym*)addUintPtr2(fHeader, dyn->d_un.d_val);
				lnk::log("fSymtab = %p", fSymtab);
				break;
			}
			default:
			{
				lnk::log("unknown dyn cmd: {tag=%p, val=%p}", dyn->d_tag, dyn->d_un.d_val);
				break;
			}
		}
		
		dyn++;
	}
	
	readReloc(fRel, fRelSize);
	readReloc(fPltRel, fPltRelSize);
}

ElfObject* ElfObject::instantiateFromFile(const char* path, const char* origPath, int fd, const struct stat* info)
{
	/*
	 XXX: This function needs ELF sanity checks.
	 */
	
	lnk::log("institating an ElfObject from file\n"
			 "            path  : %s\n"
			 "            fd    : %p"
			 , path, fd);
	
	/* read in the header */
	Elf32_Ehdr header;
	memset(&header, 0, sizeof(header));
	read_at(fd, &header, sizeof(header), 0);
	
	if (header.e_ident[EI_MAG0] != ELFMAG0 ||
		header.e_ident[EI_MAG1] != ELFMAG1 ||
		header.e_ident[EI_MAG2] != ELFMAG2 ||
		header.e_ident[EI_MAG3] != ELFMAG3) 
	{
		lnk::halt("elf instantiateFromFile(%s) invalid magic", path);
	}
	
	/* program header entry */
	uint32_t total_phentsize = header.e_phentsize * header.e_phnum;
	uint32_t total_shentsize = header.e_shentsize * header.e_shnum;
	
	void* phe = malloc(total_phentsize);
	read_at(fd, phe, total_phentsize, header.e_phoff);
	
	void* she = malloc(total_shentsize);
	read_at(fd, she, total_shentsize, header.e_shoff);
	
	/* parse the header entry */
	size_t offset = 0;
	uint32_t ncmds = header.e_phnum;
	uint32_t vmsize = 0;
	uint32_t max_vaddr = 0;
	uint32_t min_vaddr = 0;
	uint8_t* addr = (uint8_t*)phe;
	Elf32_Dyn* dyn = NULL;
	
	/*
		Precalculate the total size of the memory range that is required
		for this ELF object. This is done to avoid having to extend the
		mappings later on for the BSS bits.
	 */
	while (ncmds--) {
		/* Phdr pointer */
		Elf32_Phdr *phdr = 
		(Elf32_Phdr*)(addr + offset);
		
		if (phdr->p_type == PT_LOAD) {
			if ((phdr->p_vaddr + phdr->p_memsz) > max_vaddr)
			{
				max_vaddr = phdr->p_vaddr + phdr->p_memsz;
			}
			if (phdr->p_vaddr < min_vaddr)
			{
				min_vaddr = phdr->p_vaddr;
			}
		}
		
		offset += header.e_phentsize;
	}
	
	/* round up */
	min_vaddr &= ~PAGE_MASK;
	max_vaddr = (max_vaddr + PAGE_SIZE - 1) & ~PAGE_MASK;
	vmsize = max_vaddr - min_vaddr;
	
	void* rs = lnk::mm::reserve(vmsize);
	lnk::log("elf, reserved range {start=%p, end=%p}", rs, (void*)(vmsize + (char*)rs));
	
	/*
		Now, actually map stuff in.
	 */
	
	
	offset = 0;
	ncmds = header.e_phnum;
	while (ncmds--) {
		/* Phdr pointer */
		Elf32_Phdr *phdr = 
		(Elf32_Phdr*)(addr + offset);
		
		if (phdr->p_type == PT_LOAD) {
			lnk::log("elf, wire {vm=%p, fileoff=%p, size=%d}", (void*)addUintPtr2(rs, phdr->p_vaddr),
					 (void*)phdr->p_offset, (size_t)phdr->p_filesz);
			
			lnk::mm::wire(fd,
						  (void*)addUintPtr2(rs, phdr->p_vaddr),
						  (size_t)phdr->p_filesz,
						  (uint32_t)phdr->p_offset);
		}
		else if (phdr->p_type == PT_DYNAMIC) {
			dyn = (Elf32_Dyn*)addUintPtr2(rs, phdr->p_vaddr);
		}
		
		offset += header.e_phentsize;
	}
	
	lnk::log("elf, PT_DYNAMIC @ %p", dyn);
	
	ElfObject* obj = new ElfObject();
	obj->fSlide = (uintptr_t)rs;
	obj->fDynamic = dyn;
	obj->fFilePath = path;
	
	/* Let's assume the first load command uses fileoff of 0x0
	   Otherwise things will break.
	 */
	obj->fHeader = (Elf32_Ehdr*)rs;
	
	obj->parseDynamicSegment();
	
	return NULL;
}
