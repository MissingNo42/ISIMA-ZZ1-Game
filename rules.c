//
// Created by Romain on 03/07/2023.
//
#include <math.h>
#include <stdlib.h>

#include "rules.h"


/**
 * @brief check if a status
 * */
int match(Rule * status, Rule * rule) {
	int r = 1;
	if (status->ally.dir != JOKER && rule->ally.dir != JOKER) r &= status->ally.dir == rule->ally.dir;
	if (r && status->ally.dist != ANY && rule->ally.dist != ANY) r &= status->ally.dist == rule->ally.dist;
	
	if (r && status->prey.dir != JOKER && rule->prey.dir != JOKER) r &= status->prey.dir == rule->prey.dir;
	if (r && status->prey.dist != ANY && rule->prey.dist != ANY) r &= status->prey.dist == rule->prey.dist;
	
	if (r && status->predator.dir != JOKER && rule->predator.dir != JOKER) r &= status->predator.dir == rule->predator.dir;
	if (r && status->predator.dist != ANY && rule->predator.dist != ANY) r &= status->predator.dist == rule->predator.dist;
	return r;
}

/**
 * @brief select the rule to execute
 * @param [in] status the current status of the individual
 * @param [in] brain the brain associated to the individual
 * @return the rule to execute or -1 if none
 * */
int choice_rule(Rule * status, Brain brain){
	float probaN = 0, probaD = 0;
	int matched[P];
	for (int i = 0; i < P; i++) {
		matched[i] = match(status, brain + i);
		if (matched[i]) probaD += powf(brain[i].priority + 1, ProbaExp);
	}
	int select = rand();
	
	int last = -1;
	for (int i = 0; i < P; i++) {
		if (matched[i]) {
			last = i;
			probaN += powf(brain[i].priority + 1, ProbaExp);
			if (select * probaD <= probaN * (float)RAND_MAX) return i;
		}
	}
	return last;
}
