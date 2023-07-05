//
// Created by Romain on 03/07/2023.
//

#ifndef ZZ1GAME_RULES_H
#define ZZ1GAME_RULES_H

#define P 24
#define MAX_PRIORITY 5
#define ProbaExp 2
#define BrainPool2 30
#define BrainPool1 6
#define ITE_MAX 2000

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

typedef struct {
	Rule rules[P];
	float eval;
} Brain;

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
int choice_rule(Rule * status, Brain * brain);

/**
 * @brief modify a rule to randomize it
 * @param [in] rule a pointer to the rule
 * */
void rand_rule (Rule * rule);

/**
 * @brief modify a brain to randomize it
 * @param [in] brain a pointer to the brain
 * */
Brain * rand_brain(Brain * brain);

/**
 * @brief copy the src
 * @param [in] src the source brain
 * @param [out] dst the destination brain
 * @return the destination brain
 * @note if dst is NULL, dst is alloc
 * */
Brain * copy_brain(Brain * src, Brain * dst) ;

void printBrain(Brain * brain);

#endif //ZZ1GAME_RULES_H
