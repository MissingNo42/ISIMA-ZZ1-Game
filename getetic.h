//
// Created by lanth on 06/07/2023.
//

#ifndef ZZ1GAME_GETETIC_H
#define ZZ1GAME_GETETIC_H

#define NB_BRAINS_CANDIDATE 80
#define NB_BRAINS_COMPETING 5
#define NB_BRAINS_RECOVERED 4

#include "population.h"
#include "rules.h"

typedef struct {
    Brain *brain[NB_BRAINS_CANDIDATE];
    Brain prey, predator;
    int level; // generation
    Species species;
} Brains_gen;

void tournament(Brains_gen  * brains);

void change_brains_order(Brains_gen * brains);

void select_best_gen(Brains_gen * brains, int pos);

void proba_calculate(float * list, float prob, int nb_max);

void mutate(Brains_gen * brains, float * proba, int nb_max);

void mutate1(Brain * brain);

#endif //ZZ1GAME_GETETIC_H
