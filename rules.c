//
// Created by Romain on 03/07/2023.
//
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rules.h"


/**
 * @brief check if a status match the given rule
 * @param [in] status the status
 * @param [in] rule the rule
 * @return 1 if the status matches the rule, 0 otherwise
 * */
int match(Rule * status, Rule * rule) {
	for (int x = 0; x < 6; x++) if (status->raw[x] != -1 && rule->raw[x] != -1 && status->raw[x] != rule->raw[x]) return 0;
	return 1;
}

/**
 * @brief select the rule to execute
 * @param [in] status the current status of the individual
 * @param [in] brain the brain associated to the individual
 * @return the rule to execute or -1 if none
 * */
int choice_rule(Rule * status, Brain * brain){
	float probaN = 0, probaD = 0;
	int matched[P];
	for (int i = 0; i < P; i++) {
		matched[i] = match(status, brain->rules + i);
		if (matched[i]) probaD += powf(brain->rules[i].priority + 1, ProbaExp);
	}
	int select = rand();
	
	int last = -1;
	for (int i = 0; i < P; i++) {
		if (matched[i]) {
			last = i;
			probaN += powf(brain->rules[i].priority + 1, ProbaExp);
			if (select * probaD <= probaN * (float)RAND_MAX) return i;
		}
	}
	return last;
}

/**
 * @brief modify a rule to randomize it
 * @param [in] rule a pointer to the rule
 * */
void rand_rule(Rule * rule) {
	rule->raw[0] = rand() % 5 - 1;
	rule->raw[1] = rand() % 4 - 1;
	rule->raw[2] = rand() % 5 - 1;
	rule->raw[3] = rand() % 4 - 1;
	rule->raw[4] = rand() % 5 - 1;
	rule->raw[5] = rand() % 4 - 1;
	rule->raw[6] = rand() % 4;
	rule->raw[7] = rand() % (MAX_PRIORITY + 1);
}

/**
 * @brief modify a brain to randomize it
 * @param [in] brain a pointer to the brain
 * */
Brain * rand_brain(Brain * brain) {
    if (!brain) brain = malloc(sizeof(Brain));
    if (brain) {
        brain->eval = 0;
        for (int i = 0; i < P - 1; i++) {
            rand_rule(&brain->rules[i]);
        }
        brain->rules[P - 1] = (Rule) {.raw = {-1, -1, -1, -1, -1, -1, -1, 0}};

    }
    return brain;
}

/**
 * @brief copy the src
 * @param [in] src the source brain
 * @param [out] dst the destination brain
 * @return the destination brain
 * @note if dst is NULL, dst is alloc
 * */
Brain * copy_brain(Brain * src, Brain * dst) {
	if (!dst) dst = malloc(sizeof (Brain));
	if (dst) memcpy(dst, src, sizeof(Brain));
	return dst;
}

void printBrain(Brain * brain){
    char dir[] = {'*', 'N', 'E', 'S', 'W'};
    char dist[] = {'*', 'A', 'M', 'N'};
    printf("Brain :\n");
    for(int i = 0; i < P; i++) {
        printf("    R%d : [", i);
        for(int j = 0; j < 5; j++) {
            if(i%2) printf("%c,", dist[brain->rules[i].raw[j]]);
            else printf("%c,", dir[brain->rules[i].raw[j]]);
        }
        printf("%c] -> %c (%d)", dist[brain->rules[i].raw[5]], dir[brain->rules[i].raw[6]], brain->rules[i].raw[7]);
    }
    printf("        Eval = %f\n", brain->eval);
}
/*
#ifdef TESTING
int main(){
	Rule s = {.raw={-1, -1, -1, -1, -1, -1}};
	Rule r = {.raw={-1, -1, -1, -1, -1, -1}};
	
	printf("match 1: %d\n", match(&s, &r));
	
	s = (Rule){.raw={1,1,1,1,1,1}};
	r = (Rule){.raw={-1, -1, -1, -1, -1, -1}};
	printf("match 1: %d\n", match(&s, &r));
	
	s = (Rule){.raw={1,1,1,1,1,1}};
	r = (Rule){.raw={-1, -1, -1, -1, -1, -1}};
	printf("match 1: %d\n", match(&s, &r));
	
	s = (Rule){.raw={-1, 1, 1, 2, -1, 1}};
	r = (Rule){.raw={-1, 1, 1, 2, -1, 1}};
	printf("match 1: %d\n", match(&s, &r));
	
	s = (Rule){.raw={-1, 0, 1, 2, -1, 1}};
	r = (Rule){.raw={-1, 1, 1, 2, -1, 1}};
	printf("match 0: %d\n", match(&s, &r));
	
	s = (Rule){.raw={0, 1, 1, 2, -1, 1}};
	r = (Rule){.raw={-1, 1, 1, 2, -1, 1}};
	printf("match 1: %d\n", match(&s, &r));
	
	s = (Rule){.raw={0, 1, 1, 0, -1, 1}};
	r = (Rule){.raw={-1, 1, 1, 2, -1, 1}};
	printf("match 0: %d\n", match(&s, &r));
	
}
#endif
*/