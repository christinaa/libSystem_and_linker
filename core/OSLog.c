/*
 * Core Framework: OSLog.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Shared logging routines used throughout Core.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include "OSLog.h"

/*
	Setting this to 1 will shut up both OSLog and lnk::log.
	However, critical errors will still show up (OSHalt and lnk::halt).
 
	Set this to '1' in production mode.
 */
int __SilentMode = 0;


/*
	Pretty colours!
	Set this preprocessor directive to 1 in order to enable
	colourful logging. 
 
	(note to self: use the US version of colour)
 */
#define enablePrettyColors 1
#define default_attributes "\33[0m"

void OSTermResetAttributes(void)
{
	printf("%s", default_attributes);
}

void OSTermSetTextColor(int attr, int fg, int bg)
{
#if (enablePrettyColors)
	char command[13];
	
	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
#else
	/* No pretty colours :( */
	return;
#endif
}

void __printMessageInPrettyColors(void)
{
	/*
		Completely unnecessary.
		Don't use this in productiom.
	 */
	
	OSTermSetTextColor(BRIGHT, GREEN, BLACK);printf("P");
	OSTermSetTextColor(BRIGHT, RED, BLACK);printf("R");
	OSTermSetTextColor(BRIGHT, BLUE, BLACK);printf("E");
	OSTermSetTextColor(BRIGHT, CYAN, BLACK);printf("T");
	OSTermSetTextColor(BRIGHT, MAGENTA, BLACK);printf("T");
	OSTermSetTextColor(BRIGHT, YELLOW, BLACK);printf("Y");
	OSTermSetTextColor(BRIGHT, WHITE, BLACK);printf(" C");
	OSTermSetTextColor(BRIGHT, YELLOW, BLACK);printf("O");
	OSTermSetTextColor(BRIGHT, MAGENTA, BLACK);printf("L");
	OSTermSetTextColor(BRIGHT, CYAN, BLACK);printf("O");
	OSTermSetTextColor(BRIGHT, BLUE, BLACK);printf("U");
	OSTermSetTextColor(BRIGHT, RED, BLACK);printf("R");
	OSTermSetTextColor(BRIGHT, GREEN, BLACK);printf("S");
	
	OSTermSetTextColor(RESET, WHITE, BLACK);
	
	printf("\n");
	fflush(stdout);
}

void _OSLogInit(void)
{
	/*
		Initializer for OSLog.
	 */
	
	//__printMessageInPrettyColors();
}

void OSHalt(const char* format, ...) 
{
	OSTermSetTextColor(BRIGHT, RED, VT_DEFAULT);
	printf(" *** HALT:  ");
	OSTermSetTextColor(RESET, RED, VT_DEFAULT);
	va_list	list;
	va_start(list, format);
	vfprintf(stdout, format, list);
	va_end(list);
	
	printf("\n");
	
	OSTermSetTextColor(BRIGHT, RED, VT_DEFAULT);
	printf(" *** Invoking 'trap'  \n");
	
	OSTermResetAttributes();
	
	fflush(stdout);
	
	/* die */
	__builtin_trap();
	_exit(1); /* exit */
}

void OSLog(const char* format, ...) 
{
	if (__SilentMode)
		return;
	
	OSTermSetTextColor(LOG_FLAG, LOG_OS_COLOR, VT_DEFAULT);	
	printf("[*]: ");
	OSTermResetAttributes();
	
	va_list	list;
	va_start(list, format);
	vfprintf(stdout, format, list);
	va_end(list);
	
	printf("\n");
	
	fflush(stdout);
}

void OSLogLib(const char* library, const char* format, ...) 
{
	if (__SilentMode)
		return;
	
	OSTermSetTextColor(LOG_FLAG, LOG_OS_COLOR, VT_DEFAULT);
	printf("[%s]: ", library);
	OSTermResetAttributes();
	
	va_list	list;
	va_start(list, format);
	vfprintf(stdout, format, list);
	va_end(list);
	
	printf("\n");
	
	fflush(stdout);
}

// newline! \n

