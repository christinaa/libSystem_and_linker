/*
 * core: syslog.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Syslog functions. Not too bothered about this for now.
 */

#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include "OSLog.h"

void closelog(void)
{
	
}

void openlog(const char *ident, int logopt, int facility)
{
	
}

int	setlogmask(int maskpri)
{
	return 0;
}

void syslog(int priority, const char *message, ...)
{
	OSLogLib("syslog", "%d: %s", priority, message);
}

void vsyslog(int priority, const char *message, va_list args)
{
	
}

// newline! \n

