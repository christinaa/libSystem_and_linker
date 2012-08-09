/*
 * Core Framework: Linker: macho.h
 * Copyright (c) 2012 Christina Brooks
 *
 * Some support types for macho file handling.
 */

#ifndef core_macho_h
#define core_macho_h

#include <sys/types.h>

#define LC_SEGMENT_COMMAND	LC_SEGMENT

/* Shorthand types */
typedef struct mach_header macho_header;
typedef struct nlist macho_nlist;
typedef struct segment_command macho_segment_command;
typedef struct section macho_section;
typedef struct routines_command macho_routines_command;

typedef struct {
	void* addr;
	void* inImage;
} Symbol;

#define RELOC_SIZE				2
#define POINTER_RELOC GENERIC_RELOC_VANILLA

struct arm_thread_state {
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t r12;
	uint32_t r13; /* sp */
	uint32_t r14; /* lr */
	uint32_t r15; /* pc */
	uint32_t r16; /* cpsr */
};

struct arm_thread_command {
	uint32_t	cmd;		/* LC_THREAD or  LC_UNIXTHREAD */
	uint32_t	cmdsize;	/* total size of this command */
	uint32_t	flavor;
	uint32_t	count;
	
	struct arm_thread_state state;
};

#endif
