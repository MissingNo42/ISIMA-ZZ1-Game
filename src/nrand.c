//
// Created by Romain on 06/07/2023.
//
/**
 * @file nrand.c
 * @brief mutex-less reimplementation of rand
 * */

#include "nrand.h"

static long holdrand;

void snrand (unsigned int seed)
{
	holdrand = (long)seed;
}

int nrand (void)
{
    return(int)(((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}

