/*
 * Environ.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Environment. Adapted from Apple.
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <crt_externs.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <malloc/malloc.h>

char* gEnviron[1024] = {0}; /* XXX: derp */
char*** gEnvironPtr = (char***)&gEnviron;

char ***_NSGetEnviron(void) {
    return gEnvironPtr;
}

malloc_zone_t *__zone0;

int
init__zone0(int should_set_errno)
{
	if (__zone0) return (0);
    
	__zone0 = malloc_create_zone(0, 0);
	if (!__zone0) {
		if (should_set_errno) {
			errno = ENOMEM;
		}
		return (-1);
	}
	malloc_set_zone_name(__zone0, "environ");
	return (0);
}

char*
__findenv(name, offset, environ)
const char *name;
int *offset;
char **environ;
{
	int len, i;
	const char *np;
	char **p, *cp;
    
	if (name == NULL || environ == NULL)
		return (NULL);
	for (np = name; *np && *np != '='; ++np)
		continue;
	len = np - name;
	for (p = environ; (cp = *p) != NULL; ++p) {
		for (np = name, i = len; i && *cp; i--)
			if (*cp++ != *np++)
				break;
		if (i == 0 && *cp++ == '=') {
			*offset = p - environ;
			return (cp);
		}
	}
	return (NULL);
}

char*
_getenvp(const char *name, char ***envp, void *state __unused)
{
	int offset;
    
	return (__findenv(name, &offset, *envp));
}

char*
getenv(name)
const char *name;
{
	int offset;
    
	return (__findenv(name, &offset, *_NSGetEnviron()));
}