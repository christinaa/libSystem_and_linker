/*
 * Core Framework: ClangRtGlue.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Glue code to get the Clang runtime to work.
 */

#include <stdio.h>
#include "OSLog.h"

/**/
void abort(void);

/* something something ... */
__attribute__((weak))
__attribute__((visibility("hidden")))
void compilerrt_abort_impl(const char *file, int line, const char *function) {
	OSHalt("compilerrt_abort_impl");
}

/*
	Math glue to glue the Clang builtins to the Clang runtime.
 */
#include <math.h>
double fabs(double f) { /* faaaaabulous! */
	return __builtin_fabs(f);
}

float fabsf(float f) {
	return __builtin_fabsf(f);
}

long double fabsl(long double f) {
	return __builtin_fabsl(f);
}

long double copysignl(long double x, long double y) {
	return __builtin_copysignl(x, y);
}

float copysignf(float x, float y) {
	return __builtin_copysignf(x, y);
}

double copysign(double x, double y) {
	return __builtin_copysign(x, y);
}

// newline! \n

