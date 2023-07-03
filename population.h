//
// Created by Romain on 03/07/2023.
//

#ifndef ZZ1GAME_POPULATION_H
#define ZZ1GAME_POPULATION_H

#include "rules.h"

#define IndividualPerPopulation 5

typedef enum {
	RED = 1,
	GREEN = 2,
	BLUE = 3,
} Species;

typedef struct {
	Rule status;
	int x, y, action;
} Individual;

typedef struct {
	Individual individuals[IndividualPerPopulation];
	Brain * brain;
	Species species;
} Population;

#endif //ZZ1GAME_POPULATION_H
