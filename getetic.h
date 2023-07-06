//
// Created by lanth on 06/07/2023.
//

#ifndef ZZ1GAME_GETETIC_H
#define ZZ1GAME_GETETIC_H

#define NB_BRAINS_CANDIDATE 80

typedef struct {
    Brain *brain[NB_BRAINS_CANDIDATE];
    Brain prey, predator;
    int level; // generation
    Species species;
} Brains_gen;



#endif //ZZ1GAME_GETETIC_H
