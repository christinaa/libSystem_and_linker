/*
 * Core Framework: AuxiliaryGlue.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Glue to get the linker running before the auxiliary libs
 * are mapped in. After they're mapped, the stublist struct is
 * filled out so the functions point to the right locations.
 */

#include <sys/types.h>
#include <unistd.h>
#include <mach/message.h>
#include <time.h>
#include "OSLog.h"

typedef struct {

    double (*logb)(double x);
	double (*fmax)(double x, float y);
	double (*fmaxf)(float x, float y);
	float (*scalbnf)(float x, int y);
	float (*logbf)(float x);
	long double (*scalbnl)(long double x, int y);
	long double (*fmaxl)(long double x, long double y);
	long double (*logbl)(long double x);
	double (*scalbn)(double x, int y);
	
} lnk_stublist_t;

lnk_stublist_t lnk_stubs;

extern void _ZN3lnk4haltEPKcz(const char* format, ...) __attribute__((noreturn));
#define __halt _ZN3lnk4haltEPKcz

/*
	Stubs from libmath.
 */
double logb(double x) {
	__halt("$$$ logb $$$");
	return lnk_stubs.logb(x);
}

double fmax(double x, double y) {
	__halt("$$$ fmax $$$");
	return lnk_stubs.fmax(x, y);
}

float fmaxf(float x, float y) {
	__halt("$$$ fmaxf $$$");
	return lnk_stubs.fmaxf(x, y);
}

float scalbnf(float x, int y) {
	__halt("$$$ scalbnf $$$");
	return lnk_stubs.scalbnf(x, y);
}

float logbf(float x) {
	__halt("$$$ logbf $$$");
	return lnk_stubs.logbf(x);
}

long double scalbnl(long double x, int y) {
	__halt("$$$ scalbnl $$$");
	return lnk_stubs.scalbnl(x, y);
}

long double fmaxl(long double x, long double y) {
	__halt("$$$ fmaxl $$$");
	return lnk_stubs.fmaxl(x, y);
}

long double logbl(long double x) {
	__halt("$$$ logbl $$$");
	return lnk_stubs.logbl(x);
}

double scalbn(double x, int y) {
	__halt("$$$ scalbn $$$");
	
	return lnk_stubs.scalbn(x, y);
}


/*
	Random shit that actually needs implementing.
 */

int
_UNIMPL___tolower(int c) {
	__halt("$$$ tolower $$$");
}

#include <string.h>

int
fchmod(int fildes, int mode) {
	__halt("$$$ fchmod $$$");
}

u_int32_t
_UNIMPL__arc4random(void) {
	__halt("$$$ arc4random $$$");
}

mode_t
_UNIMPL__umask(mode_t cmask)
{
	__halt("$$$ umask $$$");
}

int
issetugid(void)
{
	/*
		Well, I don't know how this works.
		Linux specs say nothing. I'm guessing this is a BSD only func.
	 */
	
	return 0;
}

// newline! \n

