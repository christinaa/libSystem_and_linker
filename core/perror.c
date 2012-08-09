/*
 * Core Framework: Libc: perror.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Print error.
 */

#include <errno.h>
#include <unistd.h>
#include <string.h>

void perror(const char *prefix)
{
	/*
	 * Doesn't use printf for the sake of being able to use it
	 * very early and without buffered IO.
	 */
	
    char buff[256];

    strerror_r(errno, buff, sizeof(buff));

    if (prefix) {
        write(2, prefix, strlen(prefix));
        write(2, ": ", 2 );
    }
	
    write(2, buff, strlen(buff));
    write(2, "\n", 1);
}
