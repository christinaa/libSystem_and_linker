/*
 * Core Framework: Libc: LX_fcntl.h
 * Copyright (c) 2012 Christina Brooks
 *
 * Linux fcntl constants.
 */

#ifndef core_LX_fcntl_h
#define core_LX_fcntl_h

#if (__arm__)
	/* derp */
	#define LX_O_DIRECTORY 040000  
	#define LX_O_NOFOLLOW 0100000  
	#define LX_O_DIRECT 0200000  
	#define LX_O_LARGEFILE 0400000
#else
	#define LX_O_DIRECT 00040000  
	#define LX_O_LARGEFILE 00100000
	#define LX_O_DIRECTORY 00200000  
	#define LX_O_NOFOLLOW 00400000  
#endif

#define LX_O_ACCMODE 00000003
#define LX_O_RDONLY 00000000
#define LX_O_WRONLY 00000001
#define LX_O_RDWR 00000002

#define LX_O_CREAT 00000100  
#define LX_O_EXCL 00000200  
#define LX_O_NOCTTY 00000400  
#define LX_O_TRUNC 00001000  
#define LX_O_APPEND 00002000
#define LX_O_NONBLOCK 00004000
#define LX_O_SYNC 00010000
#define LX_FASYNC 00020000  

#define LX_O_NOATIME 01000000

#define LX_O_NDELAY LX_O_NONBLOCK

#endif
