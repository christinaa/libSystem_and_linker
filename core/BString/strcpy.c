/*
 * Core Framework: Libc: strcpy.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Strcpy which uses memcpy which is faster than pointer copying.
 */

#include <string.h>

/* ugh */
#undef strcpy

char *
strcpy(char *to, const char *from)
{
	/*
		Because of a simple mistake in this function, I had to
		do 7 hours worth of debugging. The original code used:
	 
		memcpy(to, from, strlen(from));
	 
		This left out the NULL terminator and fucked everything up. This
		comment is left here to remind myself that I'm a moron.
	 */
	
	memcpy(to, from, strlen(from) + 1);
	return to;
}
