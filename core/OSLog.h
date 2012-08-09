/*
 * Core Framework: OSLog.h
 * Copyright (c) 2012 Christina Brooks
 *
 * Shared logging routines used throughout Core.
 */

#ifndef core_Log_h
#define core_Log_h

#ifdef __cplusplus
extern "C" {
#endif
	
	extern int __SilentMode;
	
#define RESET		0
#define BRIGHT 		1
#define DIM		2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8
	
#define BLACK 		0
#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7
	
#define VT_DEFAULT  9
	
#define LOG_FLAG RESET
#define LOG_OS_COLOR BLUE
#define LOG_LNK_COLOR CYAN
	
	void OSHalt(const char* format, ...) __attribute__((noreturn));
	void OSLog(const char* format, ...);
	void OSLogLib(const char* library, const char* format, ...);
	void OSTermSetTextColor(int attr, int fg, int bg);
	void OSTermResetAttributes(void);
	
#define OSHaltNotImplemented() OSHalt("%s not implemented", __PRETTY_FUNCTION__) 
	
#ifdef __cplusplus	
}
#endif

#endif
