/*
 * NSCrtExterns.c
 * Copyright (c) 2012 Christina Brooks
 *
 * NeXT style access to some of the CRT values.
 */

#include <stdio.h>
#include <string.h>
#include "OSLog.h"

/*
	This is ugly.
 */

char* gArgv[1024];

int gArgc;
char*** gArgvPtr = (char***)&gArgv;

char ***_NSGetArgv(void)
{
	char*** res = (char***)&gArgvPtr;
	return (res);
}

int* _NSGetArgc(void)
{
	return &gArgc;
}
