/*
 * Core Framework: Linker: lnk_util.cpp
 * Copyright (c) 2012 Christina Brooks
 *
 * Accessing lnk::log from C code.
 */

#ifndef core__lnk_log_c_h
#define core__lnk_log_c_h

extern void _ZN3lnk3logEPKcz(const char* format, ...);
#define lnkLog _ZN3lnk3logEPKcz

extern void _ZN3lnk4ldbg11printNumberEPKci(const char* ch, int num);
#define lnkPnum _ZN3lnk4ldbg11printNumberEPKci

#endif
