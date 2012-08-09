/* go away, shitty locales */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)strcasecmp.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
#include "xlocale_private.h"

#include <strings.h>
#include <ctype.h>

typedef unsigned char u_char;

int
strcasecmp_l(s1, s2, loc)
	const char *s1, *s2;
	locale_t loc;
{
	/* ignoring locales */
	return strcasecmp(s1, s2);
}

// int strcasecmp(const char *s1, const char *s2);

int
strncasecmp_l(s1, s2, n, loc)
	const char *s1, *s2;
	size_t n;
	locale_t loc;
{
    /* ignoring locales */
	return strncasecmp(s1, s2, n);
}

// int strncasecmp(const char *s1, const char *s2, size_t n);
