/*
 * Core Framework: FakeLocale.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Well, it's not like anyone uses this.
 */

#include "OSLog.h"

#include <stdio.h>
#include <xlocale.h>
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "local.h"
#include <langinfo.h>
#include <runetype.h>
#include <inttypes.h>

#define LocaleNotImplemented() OSLog("FakeLocale: (%s:%d) not implemented ", __FUNCTION__, __LINE__)

extern int
_none_init(_RuneLocale *rl);

void FakeLocaleInit(void)
{
	_none_init(&_DefaultRuneLocale);
}

char *
setlocale(int category, const char *locale)
{
	/*
		Not bothering logging this since this isn't trivial.
	 */
	
	//LocaleNotImplemented();
	return NULL;
}

/*
 intmax_t
 strtoimax(const char *nptr, char **endptr, int base)
 */

char* strcasestr_l(const char *a1, const char *a2, locale_t loc)
{
    return strcasestr(a1, a2);
}

intmax_t
strtoimax_l(const char *nptr, char **endptr, int base, locale_t loc)
{
    return strtoimax(nptr, endptr, base);
}

double
strtod_l(const char * restrict nptr, char ** restrict endptr, locale_t loc)
{
	return strtod(nptr, endptr);
}

float
strtof_l(const char * restrict nptr, char ** restrict endptr, locale_t loc)
{
	return strtof(nptr, endptr);
}

long double
strtold_l(const char * restrict nptr, char ** restrict endptr, locale_t loc)
{
	return strtold(nptr, endptr);
}

long
strtol_l(const char * restrict nptr, char ** restrict endptr, int base, locale_t loc)
{
	return strtol(nptr, endptr, base);
}

long long
strtoll_l(const char * restrict nptr, char ** restrict endptr, int base, locale_t loc)
{
	return strtoll(nptr, endptr, base);
}

unsigned long
strtoul_l(const char * restrict nptr, char ** restrict endptr, int base, locale_t loc)
{
	return strtoul(nptr, endptr, base);
}

unsigned long long
strtoull_l(const char * restrict nptr, char ** restrict endptr, int base, locale_t loc)
{
	return strtoull(nptr, endptr, base);
}

char *
nl_langinfo(nl_item item)
{
	return "";
}

int
asprintf_l(char **ret, locale_t loc, const char *format, ...) {
	LocaleNotImplemented();
	return 0;
}

int
dprintf_l(int fd, locale_t loc, const char * restrict format, ...) {
	LocaleNotImplemented();
	return 0;
}

/*
	Yeah, CF depends on this being implemented properly.
 */
int
fprintf_l(FILE * restrict stream, locale_t loc, const char * restrict format, ...) 
{
	int ret;
	va_list ap;
	
	va_start(ap, format);
	ret = vfprintf(stream, format, ap);
	va_end(ap);
	
	return (ret);
}

int
printf_l(locale_t loc, const char * restrict format, ...) {
	LocaleNotImplemented();
	return 0;
}

int
snprintf_l(char * restrict str, size_t n, locale_t loc, const char * restrict fmt, ...) {
	va_list ap;
	int ret;
	char dummy;
	FILE f;
	struct __sfileext fext;
	
	/* While snprintf(3) specifies size_t stdio uses an int internally */
	if (n > INT_MAX)
		n = INT_MAX;
	/* Stdio internals do not deal correctly with zero length buffer */
	if (n == 0) {
		str = &dummy;
		n = 1;
	}
	_FILEEXT_SETUP((&f), &fext);
	f._file = -1;
	f._flags = __SWR | __SSTR;
	f._bf._base = f._p = (unsigned char *)str;
	f._bf._size = f._w = n - 1;
	va_start(ap, fmt);
	ret = vfprintf(&f, fmt, ap);
	va_end(ap);
	*f._p = '\0';
	
	return (ret);
}

int
sprintf_l(char * restrict str, locale_t loc, const char * restrict format, ...) {
	OSLog("[locale]: %s", str);
	return 0;
}

#include <stdarg.h>
#include <xlocale.h>

int
vasprintf_l(char **ret, locale_t loc, const char *format, va_list ap) {
	LocaleNotImplemented();
	return 0;
}

int
vdprintf_l(int fd, locale_t loc, const char * restrict format, va_list ap) {
	LocaleNotImplemented();
	return 0;
}

int
vfprintf_l(FILE * restrict stream, locale_t loc, const char * restrict format, va_list ap) {
	LocaleNotImplemented();
	return 0;
}

int
vprintf_l(locale_t loc, const char * restrict format, va_list ap) {
	LocaleNotImplemented();
	return 0;
}

int
vsnprintf_l(char * restrict str, size_t size, locale_t loc, const char * restrict format, va_list ap) {
	LocaleNotImplemented();
	return 0;
}

int
vsprintf_l(char * restrict str, locale_t loc, const char * restrict format, va_list ap)
{
	LocaleNotImplemented();
	return 0;
}