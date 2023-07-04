//
// Created by Romain on 03/07/2023.
//

#ifndef ZZ1GAME_RULES_H
#define ZZ1GAME_RULES_H

#define P 16
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
		union {
			struct {
				Info prey, predator, ally;
			};
			Info infos[3];
		};
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

/**
 * @brief modify a rule to randomize it
 * @param [in] rule a pointer to the rule
 * */
void rand_rule (Rule * rule);

/**
 * @brief modify a brain to randomize it
 * @param [in] brain a pointer to the brain
 * */
void rand_brain (Brain brain);

#endif //ZZ1GAME_RULES_H
