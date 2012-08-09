/*
 * core: rand_compat.c
 * Copyright (c) 2012 Christina Brooks
 *
 * 
 */

#include <stdio.h>

#include "rand48.h"

#define RAND_MAX 0x7fffffff

int rand(void) {
    return (int)lrand48();
}
void srand(unsigned int __s) {
    srand48(__s);
}
long random(void)
{
    return lrand48();
}
void srandom(unsigned int __s)
{
    srand48(__s);
}

