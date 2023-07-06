//
// Created by lanth on 06/07/2023.
//

#ifndef ZZ1GAME_GETETIC_H
#define ZZ1GAME_GETETIC_H

#define NB_BRAINS_CANDIDATE 80
#define NB_BRAINS_COMPETING 5
#define NB_BRAINS_RECOVERED 4

typedef struct {
    Brain *brain[NB_BRAINS_CANDIDATE];
    Brain prey, predator;
    int level; // generation
    Species species;
} Brains_gen;

Brains_gen * create_Brains_gen(Brains_gen * brains, Species species);

void change_brains_order(Brains_gen * brains);

void tournament(Brains_gen  * brains);

void change_brains_order(Brains_gen * brains);

void select_best_gen(Brains_gen * brains, int pos);

void reproduction(Brains_gen * brains);

#endif //ZZ1GAME_GETETIC_H
