/*
 * Core Framework: Linker: lnk.cpp
 * Copyright (c) 2012 Christina Brooks
 *
 * C runtime stuff.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "OSLog.h"

/* Definitions */
int __mb_cur_max = 1;
int __LastErrorNumber = 0;



#pragma mark Error Logging
/***************************************************/
void err(int eval, const char *fmt, ...)
{
	OSTermSetTextColor(LOG_FLAG, YELLOW, BLACK);	
	printf("err: ");
	OSTermSetTextColor(RESET, WHITE, BLACK);
	
	va_list	list;
	va_start(list, fmt);
	vfprintf(stdout, fmt, list);
	va_end(list);
	
	printf("\n");
	
	fflush(stdout);
}

void errx(int eval, const char *fmt, ...)
{
	OSTermSetTextColor(LOG_FLAG, YELLOW, BLACK);	
	printf("errx: ");
	OSTermSetTextColor(RESET, WHITE, BLACK);
	
	va_list	list;
	va_start(list, fmt);
	vfprintf(stdout, fmt, list);
	va_end(list);
	
	printf("\n");
	
	fflush(stdout);
}

void warn(const char *fmt, ...)
{
	OSTermSetTextColor(LOG_FLAG, YELLOW, BLACK);	
	printf("warn: ");
	OSTermSetTextColor(RESET, WHITE, BLACK);
	
	va_list	list;
	va_start(list, fmt);
	vfprintf(stdout, fmt, list);
	va_end(list);
	
	printf("\n");
	
	fflush(stdout);
}

void warnx(const char *fmt, ...)
{
	OSTermSetTextColor(LOG_FLAG, YELLOW, BLACK);	
	printf("warnx: ");
	OSTermSetTextColor(RESET, WHITE, BLACK);
	
	va_list	list;
	va_start(list, fmt);
	vfprintf(stdout, fmt, list);
	va_end(list);
	
	printf("\n");
	
	fflush(stdout);
}



#pragma mark Errors
/***************************************************/
int* __error(void) {
	return &__LastErrorNumber;
}



#pragma mark AtExit
/***************************************************/
int
atexit(void (*func)(void))
{
	return 0;
}


#pragma mark cxa
/***************************************************/
void __atexit_register_cleanup(void* whatever) {
	
}

int
__cxa_atexit(void (*func)(void *), void *arg, void *dso)
{
	return 0; 
}



#pragma mark logApi
/***************************************************/
int logAPIs() {
	return 0;
}

int logUnwinding() {
	return 0;
}