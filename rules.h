//
// Created by Romain on 03/07/2023.
//

#ifndef ZZ1GAME_RULES_H
#define ZZ1GAME_RULES_H

#define P 10
#define MAX_PRIORITY 5

typedef enum {
	JOKER = -1, N, E, S, W
} Dir;

typedef enum {
	ANY = -1, AWAY, MEDIUM, NEAR
} Dist;

typedef struct {
	Dir dir;
	Dist dist;
} Info;

typedef struct {
	Info prey, predator, ally;
	Dir action;
	int priority;
} Rule;

typedef Rule Brain[P];

#endif //ZZ1GAME_RULES_H
