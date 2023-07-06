//
// Created by Romain on 03/07/2023.
//
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rules.h"
#include "nrand.h"


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
	int select = nrand();
	
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
	rule->raw[0] = nrand() % 5 - 1;
	rule->raw[1] = nrand() % 4 - 1;
	rule->raw[2] = nrand() % 5 - 1;
	rule->raw[3] = nrand() % 4 - 1;
	rule->raw[4] = nrand() % 5 - 1;
	rule->raw[5] = nrand() % 4 - 1;
	rule->raw[6] = nrand() % 4;
	rule->raw[7] = nrand() % (MAX_PRIORITY + 1);
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
    char dist[] = {'*', 'A', 'M', 'C'};
    printf("Brain :\n");
    for(int i = 0; i < P; i++) {
        printf("    R%d : [", i);
        for(int j = 0; j < 5; j++) {
            if(j%2) printf("%c,", dist[brain->rules[i].raw[j]+1]);
            else printf("%c,", dir[brain->rules[i].raw[j]+1]);
        }
        printf("%c] -> %c (%d)\n", dist[brain->rules[i].raw[5]+1], dir[brain->rules[i].raw[6]+1], brain->rules[i].raw[7]);
    }
    printf("        Eval = %f\n", brain->eval);
}

Brain * defenseBrain(Brain * brain){
    if (!brain) brain = malloc(sizeof(Brain));
    if (brain) {
        brain->eval = 0;
        Rule r0 = {.raw = {-1, -1, 0, -1, -1, -1, 1, 4}};
        Rule r1 = {.raw = {-1, -1, 0, -1, -1, -1, 2, 5}};
        Rule r2 = {.raw = {-1, -1, 0, -1, -1, -1, 3, 4}};
        Rule r3 = {.raw = {-1, -1, 1, -1, -1, -1, 0, 4}};
        Rule r4 = {.raw = {-1, -1, 1, -1, -1, -1, 2, 5}};
        Rule r5 = {.raw = {-1, -1, 1, -1, -1, -1, 3, 4}};
        Rule r6 = {.raw = {-1, -1, 2, -1, -1, -1, 0, 5}};
        Rule r7 = {.raw = {-1, -1, 2, -1, -1, -1, 1, 4}};
        Rule r8 = {.raw = {-1, -1, 2, -1, -1, -1, 3, 4}};
        Rule r9 = {.raw = {-1, -1, 3, -1, -1, -1, 0, 4}};
        Rule r10 = {.raw = {-1, -1, 3, -1, -1, -1, 1, 5}};
        Rule r11 = {.raw = {-1, -1, 3, -1, -1, -1, 2, 4}};
        Rule r12 = {.raw = {-1, -1, -1, -1, 0, 0, 0, 2}};
        Rule r13 = {.raw = {-1, -1, -1, -1, 1, 0, 1, 2}};
        Rule r14 = {.raw = {-1, -1, -1, -1, 2, 0, 2, 2}};
        Rule r15 = {.raw = {-1, -1, -1, -1, 3, 0, 3, 2}};
        Rule r16 = {.raw = {-1, -1, -1, -1, 0, 2, 2, 2}};
        Rule r17 = {.raw = {-1, -1, -1, -1, 1, 2, 3, 2}};
        Rule r18 = {.raw = {-1, -1, -1, -1, 2, 2, 0, 2}};
        Rule r19 = {.raw = {-1, -1, -1, -1, 3, 2, 1, 2}};
        Rule r20 = {.raw = {0, 1, -1, -1, -1, -1, 0, 1}};
        Rule r21 = {.raw = {1, 1, -1, -1, -1, -1, 1, 1}};
        Rule r22 = {.raw = {2, 1, -1, -1, -1, -1, 2, 1}};
        Rule r23 = {.raw = {3, 1, -1, -1, -1, -1, 3, 1}};


        brain->rules[0] = r0;
        brain->rules[1] = r1;
        brain->rules[2] = r2;
        brain->rules[3] = r3;
        brain->rules[4] = r4;
        brain->rules[5] = r5;
        brain->rules[6] = r6;
        brain->rules[7] = r7;
        brain->rules[8] = r8;
        brain->rules[9] = r9;
        brain->rules[10] = r10;
        brain->rules[11] = r11;
        brain->rules[12] = r12;
        brain->rules[13] = r13;
        brain->rules[14] = r14;
        brain->rules[15] = r15;
        brain->rules[16] = r16;
        brain->rules[17] = r17;
        brain->rules[18] = r18;
        brain->rules[19] = r19;
        brain->rules[20] = r20;
        brain->rules[21] = r21;
        brain->rules[22] = r22;
        brain->rules[23] = r23;
    }
    return brain;
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
