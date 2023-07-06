//
// Created by Romain on 06/07/2023.
//

#include "nrand.h"

static long holdrand;

void snrand (unsigned int seed)
{
	holdrand = (long)seed;
}

int nrand (void)
{
    return(((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}

