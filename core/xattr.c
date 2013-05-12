/*
 * core: xattr.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Extended attributes.
 * Not bothering with these as they're HFS+ only (or something).
 */

#include <stdio.h>
#include "OSLog.h"
#include <sys/xattr.h>
#include <errno.h>

ssize_t
getxattr(const char *path, const char *name, void *value, size_t size, u_int32_t position, int options)
{
	errno = EPERM;
	return -1;
}

ssize_t
listxattr(const char *path, char *namebuf, size_t size, int options)
{
	errno = EPERM;
	return -1;
}

// newline! \n

