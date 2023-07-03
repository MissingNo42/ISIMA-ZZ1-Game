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
	Species species;
} Individual;

typedef struct {
	Individual individuals[IndividualPerPopulation];
	Brain * brain;
} Population;

void mutation (Brain brain);

void hybridization (Brain parent1, Brain parent2,Brain child);

#endif //ZZ1GAME_POPULATION_H
