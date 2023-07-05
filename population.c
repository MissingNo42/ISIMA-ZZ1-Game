//
// Created by lanth on 03/07/2023.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

#include "population.h"
#include "rules.h"
#include "field.h"

/**
 * @brief prepare the action: check environment collision and pre-perform the move in id->nx/ny
 * @param [in,out] id the individual to prepare the action
 * */
void prepare_move(Individual * id) { // possible de rajouter si 2 ally vont au meme endroit, le 2eme ne bouge pas
	id->nx = id->x;
	id->ny = id->y;
	
	if (id->action == JOKER) id->action = rand() % 4;
	switch (id->action) {
		case N: if (id->y - 1 > 0) id->ny = id->y - 1;
			break;
		case E: if (id->x + 1 < SIZEMAP) id->nx = id->x + 1;
			break;
		case S: if (id->y + 1 < SIZEMAP) id->ny = id->y + 1;
			break;
		case W: if (id->x - 1 > 0) id->nx = id->x - 1;
			break;
		default: break;
	}
}

/**
 * @brief choose an action for all individuals and prepare its execution
 * @param [in, out] pops the populations set
 * */
void predict_move(Populations * pops) {
	
	for (int p = 0; p < 3; ++p) {
		Population * pop = &pops->pops[p];
		
		for (int i = 0; i < IndividualPerPopulation; i++) {
			Individual * id = &pop->individuals[i];
			
			if (id->alive) {
				int ch = choice_rule(&id->status, pop->brain);
				id->action = (ch == -1) ? JOKER : pop->brain->rules[ch].action;
				prepare_move(id);
			}
		}
	}
}

void eat_move(Populations * populations) {
	
	for (int pop = 0; pop < 3; pop++) {
		
		for (int i = 0; i < IndividualPerPopulation; i++) {
			Population * self_pop = &populations->pops[pop];
			Individual * self = &(self_pop->individuals[i]);
			
			if (self->alive) {
				for (int j = 0; j < IndividualPerPopulation; j++) {
					Population * prey_pop = &populations->pops[(pop + 1) % 3];
					Individual * prey = &(prey_pop->individuals[j]);
					
					if (prey->alive && ((prey->nx == self->nx && prey->ny == self->ny)
					                    || ((prey->x == self->nx && prey->y == self->ny) &&
					                        (prey->nx == self->x && prey->ny == self->y)))) {
						self_pop->state.targets--;
						prey_pop->state.alives--;
						prey->alive = 0; //TODO
					}
				}
			}
		}
	}
}

void execute_move(Populations * populations) {
	for (int pop = 0; pop < 3; pop++) {
		for (int i = 0; i < IndividualPerPopulation; i++) {
			Individual * id = &(populations->pops[pop].individuals[i]);
			if (id->alive) {
				id->x = id->nx;
				id->y = id->ny;
			}
		}
	}
}

void move(Populations * pops) {
	predict_move(pops);
	eat_move(pops);
	execute_move(pops);
}

void eval(Populations * pops, int ind) {
	Population * pop = &pops->pops[ind];
	pop->brain->eval = (powf(pop->state.alives, 2)
	                    + powf(IndividualPerPopulation - pop->state.targets, 1.5)
	                    + (pop->state.end_state * IndividualPerPopulation)
	                   ) / sqrt(pops->iteration);
}

/**
 * @brief check if the game is terminated and compute result if needed
 * @param [in] pops the population set
 * @param [in, out] states the current state
 * @return 0 if the game continue, 1 if terminated and return the result
 * */
int is_terminated(Populations * pops) {
	pops->iteration++;
	int r = 0;
	
	for (int p = 0; p < 3; ++p) {
		Population * pop = &pops->pops[p];
		if (!pop->state.alives) {
			r = 1;
			pop->state.end_state = Lose;
			EndState * e = &pops->pops[(p + 2) % 3].state.end_state;
			*e = (*e != None) ? Kamikaze : Win;
		}
	}
	
	return r;
}

void mutation(Brain * brain) {
	int i = rand() % P;
	int j = rand() % 8;
	int k = rand();
	int d = 0;
	if (k > 0.1 * RAND_MAX) {
		if (k > 0.55 * RAND_MAX) d = 1;
		else d = -1;
	}
	switch (j) {
		case 0:
			if (d == 0) brain->rules[i].prey.dir = -1;
			else brain->rules[i].prey.dir = (brain->rules[i].prey.dir + d + 4) % 4;
			break;
		case 1:
			if (d == 0) brain->rules[i].prey.dist = -1;
			else brain->rules[i].prey.dist = (brain->rules[i].prey.dist + d + 3) % 3;
			break;
		case 2:
			if (d == 0) brain->rules[i].predator.dir = -1;
			else brain->rules[i].predator.dir = (brain->rules[i].predator.dir + d + 4) % 4;
			break;
		case 3:
			if (d == 0) brain->rules[i].predator.dist = -1;
			else brain->rules[i].predator.dist = (brain->rules[i].predator.dist + d + 3) % 3;
			break;
		case 4:
			if (d == 0) brain->rules[i].ally.dir = -1;
			else brain->rules[i].ally.dir = (brain->rules[i].ally.dir + d + 4) % 4;
			break;
		case 5:
			if (d == 0) brain->rules[i].ally.dist = -1;
			else brain->rules[i].ally.dist = (brain->rules[i].ally.dist + d + 3) % 3;
			break;
		case 6:
			if (d == 0) brain->rules[i].action = -1;
			else brain->rules[i].action = (brain->rules[i].action + d + 4) % 4;
			break;
		case 7:brain->rules[i].priority = (brain->rules[i].priority + d + MAX_PRIORITY) % MAX_PRIORITY;
			break;
	}
}

void hybridization(Brain * parent1, Brain * parent2, Brain * child) {
	int rul = rand() % P;
	int arg = rand() % 8;
	for (int i = 0; i < rul; i++) {
		for (int j = 0; j < 8; j++) {
			child->rules[i].raw[j] = parent1->rules[i].raw[j];
		}
	}
	for (int j = 0; j < arg; j++) {
		child->rules[rul].raw[j] = parent1->rules[rul].raw[j];
	}
	for (int j = arg + 1; j < 8; j++) {
		child->rules[rul].raw[j] = parent2->rules[rul].raw[j];
	}
	for (int i = rul + 1; i < P; i++) {
		for (int j = 0; j < 8; j++) {
			child->rules[i].raw[j] = parent2->rules[i].raw[j];
		}
	}
}

/**
 * @brief save the given brain to ./brains/<level>.<species>
 * @param [in] brain the brain to save
 * @param [in] level the level (= nb of evolution of the brain)
 * @param [in] species the species associate to the brain
 * @return 1 if saved, 0 otherwise
 * */
int save_brain(Brain * brain, int level, Species species) {
	char str[42];
	int r = 1;
	
	mkdir("brains", 0744);
	snprintf(str, 32, "brains/%06d.%s", level, (species == RED) ? "red" : ((species == BLUE) ? "blue" : "green"));
	
	FILE * f = fopen(str, "wb");
	if (f) {
		r &= fwrite(brain, sizeof(Brain), 1, f) == sizeof(Brain);
		fclose(f);
	} else r = 0;
	return r;
}

/**
 * @brief load the given brain from ./brains/<level>.<species>
 * @param [in] brain the brain to load
 * @param [in] level the level (= nb of evolution of the brain)
 * @param [in] species the species associate to the brain
 * @return 1 if loaded, 0 otherwise
 * */
int load_brain(Brain * brain, int level, Species species) {
	char str[42];
	int r = 1;
	
	snprintf(str, 32, "brains/%06d.%s", level, (species == RED) ? "red" : ((species == BLUE) ? "blue" : "green"));
	
	FILE * f = fopen(str, "rb");
	if (f) {
		r &= fread(brain, sizeof(Brain), 1, f) == sizeof(Brain);
		fclose(f);
	} else r = 0;
	return r;
}

/**
 * @brief get the last level of the saved brains of the specified species
 * @param [in] species the species
 * @return the last level or -1
 * */
int get_last_brain(Species species) {
	char extw[10];
	int lvl, lvl_max = -1;
	
	DIR * d = opendir("brains");
	struct dirent * dir;
	
	if (d) {
		snprintf(extw, 10, ".%s", (species == RED) ? "red" : ((species == BLUE) ? "blue" : "green"));
		while ((dir = readdir(d)) != NULL) {
			char * ext;
			lvl = (int) strtol(dir->d_name, &ext, 10);
			if (lvl > lvl_max && strcmp(ext, extw) == 0) lvl_max = lvl; //sscanf(dir->d_name, "%d.%*c", &lvl);
		}
		closedir(d);
	}
	return lvl_max;
}

void rand_individual(Individual * ind, Locator loc) {
	ind->alive = 1;
	ind->x;
	ind->y;
}
/*
#ifdef TESTING
int main(){
	printf("ZZZZZ\n");
	Brain b, q;
	rand_brain(b);
	save_brain(b, 42, RED);
	printf(">>%d\n", get_last_brain(RED));
	load_brain(q, get_last_brain(RED), RED);
	save_brain(q, 22, GREEN);
	printf(">>%d\n", get_last_brain(RED));
	printf(">>%d\n", get_last_brain(GREEN));
	printf(">>%d\n", get_last_brain(BLUE));
}
#endif

*/
