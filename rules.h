//
// Created by Romain on 03/07/2023.
//

#ifndef ZZ1GAME_RULES_H
#define ZZ1GAME_RULES_H

#define P 10
#define MAX_PRIORITY 5
#define ProbaExp 2

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

typedef union {
	struct {
		Info prey, predator, ally;
		Dir action;
		int priority;
	};
	int raw[8];
} Rule;

typedef Rule Brain[P];

/**
 * @brief check if a status match the given rule
 * @param [in] status the status
 * @param [in] rule the rule
 * @return 1 if the status matches the rule, 0 otherwise
 * */
int match(Rule * status, Rule * rule);

/**
 * @brief select the rule to execute
 * @param [in] status the current status of the individual
 * @param [in] brain the brain associated to the individual
 * @return the rule to execute or -1 if none
 * */
int choice_rule(Rule * status, Brain brain);

void rand_rule (Rule * rule);

#endif //ZZ1GAME_RULES_H
