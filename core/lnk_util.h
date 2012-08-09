/*
 * Core Framework: Linker: lnk_util.h
 * Copyright (c) 2012 Christina Brooks
 *
 * Linker utilities.
 */

#ifndef core_lnk_util_h
#define core_lnk_util_h

#include <sys/types.h>
#include <mach-o/nlist.h>
#include <mach-o/loader.h>

int read_at(int fd, void* buf, size_t len, uint32_t offset);
const uint8_t* trie_walk(const uint8_t* start, const uint8_t* end, const char* s);
uintptr_t read_uleb128(const uint8_t*& p, const uint8_t* end);
intptr_t read_sleb128(const uint8_t*& p, const uint8_t* end);

const struct nlist* binary_search_toc(const char* key,
									  const char stringPool[],
									  const struct nlist symbols[], 
									  const struct dylib_table_of_contents toc[],
									  uint32_t symbolCount,
									  uint32_t hintIndex);

const struct nlist* binary_search(const char* key,
								  const char stringPool[],
								  const struct nlist symbols[],
								  uint32_t symbolCount);

#endif
