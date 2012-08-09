/*
 * Core Framework: Linker: lnk_util.cpp
 * Copyright (c) 2012 Christina Brooks
 *
 * Linker utilities.
 */

#include "lnk.h"
#include "lnk_debug.h"

#include "lnk_util.h"

#include <unistd.h>

int read_at(int fd, void* buf, size_t len, uint32_t offset) {
	lseek(fd, offset, SEEK_SET);
	return read(fd, buf, len);
}

const struct nlist* binary_search_toc(const char* key,
									  const char stringPool[],
									  const struct nlist symbols[], 
									  const struct dylib_table_of_contents toc[],
									  uint32_t symbolCount,
									  uint32_t hintIndex)
{
	int32_t high = symbolCount-1;
	int32_t mid = hintIndex;

	if ( mid >= (int32_t)symbolCount )
		mid = symbolCount/2;

	for (int32_t low = 0; low <= high; mid = (low+high)/2) {
		const uint32_t index = toc[mid].symbol_index;
		const struct nlist* pivot = &symbols[index];
		const char* pivotStr = &stringPool[pivot->n_un.n_strx];

		int cmp = strcmp(key, pivotStr);
		if ( cmp == 0 )
			return pivot;
		if ( cmp > 0 ) {
			// key > pivot 
			low = mid + 1;
		}
		else {
			// key < pivot 
			high = mid - 1;
		}
	}
	return NULL;
}

const struct nlist* binary_search(const char* key,
								  const char stringPool[],
								  const struct nlist symbols[],
								  uint32_t symbolCount)
{
	const struct nlist* base = symbols;
	for (uint32_t n = symbolCount; n > 0; n /= 2) {
		const struct nlist* pivot = &base[n/2];
		const char* pivotStr = &stringPool[pivot->n_un.n_strx];

		int cmp = strcmp(key, pivotStr);
		if ( cmp == 0 )
			return pivot;
		if ( cmp > 0 ) {
			// key > pivot 
			// move base to symbol after pivot
			base = &pivot[1];
			--n; 
		}
		else {
			// key < pivot 
			// keep same base
		}
	}
	return NULL;
}


const uint8_t* trie_walk(const uint8_t* start, const uint8_t* end, const char* s)
{
	const uint8_t* p = start;
	while ( p != NULL ) {
		uint32_t terminalSize = *p++;
		if ( terminalSize > 127 ) {
			// except for re-export-with-rename, all terminal sizes fit in one byte
			--p;
			terminalSize = read_uleb128(p, end);
		}
		if ( (*s == '\0') && (terminalSize != 0) ) {
			//lnk::log("trieWalk(%p) returning %p\n", start, p);
			return p;
		}
		const uint8_t* children = p + terminalSize;
		//lnk::log("trieWalk(%p) sym=%s, terminalSize=%d, children=%d\n", start, s, terminalSize, *children);
		uint8_t childrenRemaining = *children++;
		p = children;
		uint32_t nodeOffset = 0;
		for (; childrenRemaining > 0; --childrenRemaining) {
			const char* ss = s;
			//lnk::log("trieWalk(%p) child str=%s", start, (char*)p);
			bool wrongEdge = false;
			// scan whole edge to get to next edge
			// if edge is longer than target symbol name, don't read past end of symbol name
			char c = *p;
			while ( c != '\0' ) {
				if ( !wrongEdge ) {
					if ( c != *ss )
						wrongEdge = true;
					++ss;
				}
				++p;
				c = *p;
			}
			if ( wrongEdge ) {
				// advance to next child
				++p; // skip over zero terminator
				// skip over uleb128 until last byte is found
				while ( (*p & 0x80) != 0 )
					++p;
				++p; // skil over last byte of uleb128
			}
			else {
 				// the symbol so far matches this edge (child)
				// so advance to the child's node
				++p;
				nodeOffset = read_uleb128(p, end);
				s = ss;
				//lnk::log("trieWalk() found matching edge advancing to node 0x%x", nodeOffset);
				break;
			}
		}
		if ( nodeOffset != 0 )
			p = &start[nodeOffset];
		else
			p = NULL;
	}
	//lnk::log("trieWalk(%p) return NULL", start);
	return NULL;
}

uintptr_t read_uleb128(const uint8_t*& p, const uint8_t* end)
{
	uint64_t result = 0;
	int	bit = 0;
	do {
		if (p == end)
			lnk::halt("malformed uleb128");
		
		uint64_t slice = *p & 0x7f;
		
		if (bit > 63)
			lnk::halt("uleb128 too big for uint64, bit=%d, result=0x%0llX", bit, result);
		else {
			result |= (slice << bit);
			bit += 7;
		}
	} while (*p++ & 0x80);
	return result;
}


intptr_t read_sleb128(const uint8_t*& p, const uint8_t* end)
{
	int64_t result = 0;
	int bit = 0;
	uint8_t byte;
	do {
		if (p == end)
			lnk::halt("malformed sleb128");
		byte = *p++;
		result |= ((byte & 0x7f) << bit);
		bit += 7;
	} while (byte & 0x80);
	// sign extend negative numbers
	if ( (byte & 0x40) != 0 )
		result |= (-1LL) << bit;
	return result;
}