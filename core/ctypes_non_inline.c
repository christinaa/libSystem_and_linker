/*
 * core: ctypes_non_inline.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Poof.
 */


/*
 * Always ensure this corresponds to the stuff in "ctype.h".
 * This is based on it.
 */

#include <stdio.h>
#include "xlocale_private.h"
#include <runetype.h>

/* These are normal defs */
#define __DARWIN_CTYPE_static_inline
#define __DARWIN_CTYPE_TOP_static_inline

#define	_CTYPE_A	0x00000100L		/* Alpha */
#define	_CTYPE_C	0x00000200L		/* Control */
#define	_CTYPE_D	0x00000400L		/* Digit */
#define	_CTYPE_G	0x00000800L		/* Graph */
#define	_CTYPE_L	0x00001000L		/* Lower */
#define	_CTYPE_P	0x00002000L		/* Punct */
#define	_CTYPE_S	0x00004000L		/* Space */
#define	_CTYPE_U	0x00008000L		/* Upper */
#define	_CTYPE_X	0x00010000L		/* X digit */
#define	_CTYPE_B	0x00020000L		/* Blank */
#define	_CTYPE_R	0x00040000L		/* Print */
#define	_CTYPE_I	0x00080000L		/* Ideogram */
#define	_CTYPE_T	0x00100000L		/* Special */
#define	_CTYPE_Q	0x00200000L		/* Phonogram */
#define	_CTYPE_SW0	0x20000000L		/* 0 width character */
#define	_CTYPE_SW1	0x40000000L		/* 1 width character */
#define	_CTYPE_SW2	0x80000000L		/* 2 width character */
#define	_CTYPE_SW3	0xc0000000L		/* 3 width character */
#define	_CTYPE_SWM	0xe0000000L		/* Mask for screen width data */
#define	_CTYPE_SWS	30			/* Bits to shift to get width */
#define	_A		_CTYPE_A		/* Alpha */
#define	_C		_CTYPE_C		/* Control */
#define	_D		_CTYPE_D		/* Digit */
#define	_G		_CTYPE_G		/* Graph */
#define	_L		_CTYPE_L		/* Lower */
#define	_P		_CTYPE_P		/* Punct */
#define	_S		_CTYPE_S		/* Space */
#define	_U		_CTYPE_U		/* Upper */
#define	_X		_CTYPE_X		/* X digit */
#define	_B		_CTYPE_B		/* Blank */
#define	_R		_CTYPE_R		/* Print */
#define	_I		_CTYPE_I		/* Ideogram */
#define	_T		_CTYPE_T		/* Special */
#define	_Q		_CTYPE_Q		/* Phonogram */
#define	_SW0		_CTYPE_SW0		/* 0 width character */
#define	_SW1		_CTYPE_SW1		/* 1 width character */
#define	_SW2		_CTYPE_SW2		/* 2 width character */
#define	_SW3		_CTYPE_SW3		/* 3 width character */

#define __current_locale() (_DefaultRuneLocale)

unsigned long
___runetype_l(__darwin_ct_rune_t c, locale_t loc)
{
    size_t lim;
    _RuneRange *rr;
    _RuneEntry *base, *re;
    
    if (c < 0 || c == EOF)
        return(0L);
    
    rr = &_DefaultRuneLocale.__runetype_ext;
    /* Binary search -- see bsearch.c for explanation. */
    base = rr->__ranges;
    for (lim = rr->__nranges; lim != 0; lim >>= 1) {
        re = base + (lim >> 1);
        if (re->__min <= c && c <= re->__max) {
            if (re->__types)
                return(re->__types[c - re->__min]);
            else
                return(re->__map);
        } else if (c > re->__max) {
            base = re + 1;
            lim--;
        }
    }
    
    return(0L);
}

unsigned long
___runetype(__darwin_ct_rune_t c)
{
    return ___runetype_l(c, 0);
}

__DARWIN_CTYPE_static_inline int
__maskrune(__darwin_ct_rune_t _c, unsigned long _f)
{
	return ((_c < 0 || _c >= _CACHED_RUNES) ? ___runetype(_c) :
            _DefaultRuneLocale.__runetype[_c]) & _f;
}

int __maskrune_l(__darwin_ct_rune_t a, unsigned long b, locale_t c)
{
    return __maskrune(a, b);
}

__DARWIN_CTYPE_TOP_static_inline int
isascii(int _c)
{
	return ((_c & ~0x7F) == 0);
}

__DARWIN_CTYPE_static_inline int
__istype(__darwin_ct_rune_t _c, unsigned long _f)
{
	return (isascii(_c) ? !!(_DefaultRuneLocale.__runetype[_c] & _f)
            : !!__maskrune(_c, _f));
}

__DARWIN_CTYPE_static_inline __darwin_ct_rune_t
__isctype(__darwin_ct_rune_t _c, unsigned long _f)
{
	return (_c < 0 || _c >= _CACHED_RUNES) ? 0 :
    !!(_DefaultRuneLocale.__runetype[_c] & _f);
}

__DARWIN_CTYPE_TOP_static_inline int
isalnum(int _c)
{
	return (__istype(_c, _CTYPE_A|_CTYPE_D));
}

__DARWIN_CTYPE_TOP_static_inline int
isalpha(int _c)
{
	return (__istype(_c, _CTYPE_A));
}

__DARWIN_CTYPE_TOP_static_inline int
isblank(int _c)
{
	return (__istype(_c, _CTYPE_B));
}

__DARWIN_CTYPE_TOP_static_inline int
iscntrl(int _c)
{
	return (__istype(_c, _CTYPE_C));
}

/* ANSI -- locale independent */
__DARWIN_CTYPE_TOP_static_inline int
isdigit(int _c)
{
	return (__isctype(_c, _CTYPE_D));
}

__DARWIN_CTYPE_TOP_static_inline int
isgraph(int _c)
{
	return (__istype(_c, _CTYPE_G));
}

__DARWIN_CTYPE_TOP_static_inline int
islower(int _c)
{
	return (__istype(_c, _CTYPE_L));
}

__DARWIN_CTYPE_TOP_static_inline int
isprint(int _c)
{
	return (__istype(_c, _CTYPE_R));
}

__DARWIN_CTYPE_TOP_static_inline int
ispunct(int _c)
{
	return (__istype(_c, _CTYPE_P));
}

__DARWIN_CTYPE_TOP_static_inline int
isspace(int _c)
{
	return (__istype(_c, _CTYPE_S));
}

__DARWIN_CTYPE_TOP_static_inline int
isupper(int _c)
{
	return (__istype(_c, _CTYPE_U));
}

/* ANSI -- locale independent */
__DARWIN_CTYPE_TOP_static_inline int
isxdigit(int _c)
{
	return (__isctype(_c, _CTYPE_X));
}

__DARWIN_CTYPE_TOP_static_inline int
toascii(int _c)
{
	return (_c & 0x7F);
}

int __tolower(int c);
int __toupper(int c);

__DARWIN_CTYPE_TOP_static_inline int
tolower(int _c)
{
    return (__tolower(_c));
}

__DARWIN_CTYPE_TOP_static_inline int
toupper(int _c)
{
    return (__toupper(_c));
}

// newline! \n

