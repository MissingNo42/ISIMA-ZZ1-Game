//
// Created by lanth on 03/07/2023.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <threads.h>

#include "population.h"
#include "rules.h"
#include "field.h"
#include "nrand.h"

/**
 * @brief prepare the action: check environment collision and pre-perform the move in id->nx/ny
 * @param [in,out] id the individual to prepare the action
 * */
void prepare_move(Individual * id) { // possible de rajouter si 2 ally vont au meme endroit, le 2eme ne bouge pas
	id->nx = id->x;
	id->ny = id->y;
	
	if (id->action == JOKER) id->action = nrand() % 4;
	switch (id->action) {
		case N: if (id->y - 1 >= 0) id->ny = id->y - 1;
			break;
		case E: if (id->x + 1 < SIZEMAP) id->nx = id->x + 1;
			break;
		case S: if (id->y + 1 < SIZEMAP) id->ny = id->y + 1;
			break;
		case W: if (id->x - 1 >= 0) id->nx = id->x - 1;
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
	pop->brain->eval = (powf((float)pop->state.alives, 2) - powf((float)pop->state.targets, 2)
	                   ) / sqrtf((float)pops->iteration)
                       + (float)pop->state.end_state * IndividualPerPopulation * 3;
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
			
			pop->state.end_state = (pop->state.end_state == Win) ? Kamikaze: Lose;

			EndState * e = &pops->pops[(p + 2) % 3].state.end_state;
			*e = (*e == Lose) ? Kamikaze : Win;
		}
	}
	
	return r;
}

/**
 * @brief execute a complete simulation
 * @param [in, out] pops the population to simulate
 * @return the number of iterations
 * */
int simulate(Populations * pops) {
	do {
		update_status(pops);
		move(pops);
	} while (!is_terminated(pops) && pops->iteration < ITE_MAX);
	return pops->iteration;
}

void mutation(Brain * brain) {
	int i = nrand() % P;
	int j = nrand() % 8;
	int k = nrand();
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

int mutation_one(Brains * brains, int L ) {
    int i = L % 8,
        j = L / 8;
    //printf("\tj,i,L : %d,%d,%d\n",j,i,L);
    int nb = 0;
    switch (i) {
        case 0:
        case 2:
        case 4:
            nb = 5;
            for (int k=-1; k<4;k++){
                brains->brain[k+1]->rules[j].raw[i] = k;
            }
            break;
        case 6:
            nb = 4;
            for (int k=0; k<4;k++){
                brains->brain[k]->rules[j].raw[i] = k;
            }
            break;
        case 1:
        case 3:
        case 5:
            nb = 4;
            for (int k=-1; k<3;k++){
                brains->brain[k+1]->rules[j].raw[i] = k;
            }
            break;
        case 7:
            nb = MAX_PRIORITY + 1;
            for (int k=0; k<MAX_PRIORITY + 1;k++){
                brains->brain[k]->rules[j].raw[i] = k;
            }
            break;
        default:
            break;
    }
    return nb;
}

typedef struct{
	Brains * brains;
	Species species;
	int num;
} ThArgs;

int simu_thread(void * args){
	Brains * brains = ((ThArgs *)args)->brains;
	Species species = ((ThArgs *)args)->species;
	int num = ((ThArgs *)args)->num;

	Brain *brain_list[3];
    brain_list[species - 1] = brains->brain[num];
    brain_list[species % 3] = &brains->prey;
    brain_list[(species + 1) % 3] = &brains->predator;
    float eval_val = 0;

    for(int anti_rand = 0; anti_rand<30;anti_rand++) {
        Populations *pops = create_pops(NULL, brain_list, anti_rand%3);
        simulate(pops);
        eval(pops, species - 1);
		//printf("ite: %d, eval: %f, alives: %d,targets: %d\n", pops->iteration, pops->pops[species - 1].brain->eval, pops->pops[species - 1].state.alives, pops->pops[species - 1].state.targets);
        eval_val += brains->brain[num]->eval;
        free(pops);
    }
    brains->brain[num]->eval = eval_val / 30;
    //printf("\teval : %f (%d)\n", brains->brain[num]->eval, num);

	return 0;
}

void mutation_all (Brains * brains, int* list_ind, Species species){
    change_path_random(list_ind, 8*P);
    int nb = 0;
    for (int k=0; k<P*8; k++){
        brains->level++;

        for (int m=1; m< BrainPool1; m++){
            copy_brain(brains->brain[0],brains->brain[m]);
        }

        nb = mutation_one(brains,list_ind[k]);

        //thread pour chaque brain
		thrd_t handlers[nb];
		ThArgs tha[nb];

        for (int num = 0; num< nb; num++) {
			tha[num].brains = brains;
			tha[num].species = species;
			tha[num].num = num;

	        if (thrd_success != thrd_create(handlers + num, simu_thread, tha + num)){
				printf("CRITICAL TH %d\n", num);
				exit(0);
			}
        }

		for (int num = 0; num< nb; num++) {
			thrd_join(handlers[num], NULL);
		}

        //fin threads
        select_best(brains, nb);
        //printf("\tbrain_num : %d\n",k);
    }
}

int mutation_two(Brains * brains, int  L) {
    int M = L;
    while(M==L) {
        M = nrand() % (8 * P);
    }
    int jL = L / 8, iL = L % 8;
    int jM = M / 8, iM = M % 8;
    printf("L : %d (%d|%d) |M : %d (%d|%d)\n",L,jL,iL,M,jM,iM);
    int nbL = 0, decL = 0;
    int nbM = 0, decM = 0;
    switch (iL) {
        case 0:
        case 2:
        case 4:
            nbL = 5;
            decL = 1;
            break;
        case 6:
            nbL = 4;
            break;
        case 1:
        case 3:
        case 5:
            nbL = 4;
            decL = 1;
            break;
        case 7:
            nbL = MAX_PRIORITY + 1;
            break;
        default:
            break;
    }
    switch (iM) {
        case 0:
        case 2:
        case 4:
            nbM = 5;
            decM = 1;
            break;
        case 6:
            nbM = 4;
            break;
        case 1:
        case 3:
        case 5:
            nbM = 4;
            decM = 1;
            break;
        case 7:
            nbM = MAX_PRIORITY + 1;
            break;
        default:
            break;
    }
    for (int i = 0; i<nbM; i++){
        for (int j=0; j<nbL; j++){
            brains->brain[i*nbL+j]->rules[jL].raw[iL] = j - decL;
            brains->brain[i*nbL+j]->rules[jM].raw[iM] = i - decM;
        }
    }
    return nbL*nbM;
}

int simu_thread2(void * args){
	Brains * brains = ((ThArgs *)args)->brains;
	int num = ((ThArgs *)args)->num;

    Brain *brain_list[3];
    Species species = brains->species;
    brain_list[species - 1] = brains->brain[num];
    brain_list[species % 3] = &brains->prey;
    brain_list[(species + 1) % 3] = &brains->predator;
    float eval_val = 0;
    for (int anti_rand = 0; anti_rand < 20; anti_rand++) {
        Populations *pops = create_pops(NULL, brain_list, anti_rand % 3);
        simulate(pops);
        eval(pops, species - 1);
        eval_val += brains->brain[num]->eval;
        free(pops);
    }
    brains->brain[num]->eval = eval_val / 20;
    //printf("eval : %f\n", brains->brain[num]->eval);
    return 0;
}

void mutation_two_do (Brains * brains, int* list_ind){
    change_path_random(list_ind, 8*P);
    int nb = 0;
    for (int k=0; k<P*8; k++) {
        brains->level++;

        for (int m = 1; m < BrainPool2; m++) {
            copy_brain(brains->brain[0], brains->brain[m]);
        }

        nb = mutation_two(brains, list_ind[k]);
		ThArgs tha[nb];
	    thrd_t handlers[nb];
        for (int num = 0; num < nb; num++) {
			tha[num].brains = brains;
			tha[num].num = num;

	        if (thrd_success != thrd_create(handlers + num, simu_thread2, tha + num)){
				printf("CRITICAL TH %d\n", num);
				exit(0);
			}
        }

		for (int num = 0; num< nb; num++) {
			thrd_join(handlers[num], NULL);
		}

        select_best(brains, nb);
        printf("nb : %d\t",nb);
        printf("eval : %f\n", brains->brain[0]->eval);
    }
}

void select_best(Brains * brains, int nb){
    int j = 0;
    float eval = brains->brain[0]->eval;
    for (int k = 1; k<nb; k++){
        if (eval < brains->brain[k]->eval){
            j = k;
            eval = brains->brain[k]->eval;
        }
    }
    if (j){
        Brain * tmp = brains->brain[j];
        brains->brain[j] = brains->brain[0];
        brains->brain[0] = tmp;
    }
}

void change_path_random(int * list, int size){
    for (int k = size ; k>0; k--){
        int i = nrand() % k;
        int tmp = list[k-1];
        list[k-1] = list[i];
        list[i] = tmp;
    }
}


void hybridization(Brain * parent1, Brain * parent2, Brain * child) {
	int rul = nrand() % P;
	int arg = nrand() % 8;
	for (int i = 0; i < rul; i++) {
		for (int j = 0; j < 8; j++) {
			child->rules[i].raw[j] = parent1->rules[i].raw[j];
		}
	}
	for (int j = 0; j < arg; j++) {
		child->rules[rul].raw[j] = parent1->rules[rul].raw[j];
	}
	for (int j = arg; j < 8; j++) {
		child->rules[rul].raw[j] = parent2->rules[rul].raw[j];
	}
	for (int i = rul + 1; i < P; i++) {
		for (int j = 0; j < 8; j++) {
			child->rules[i].raw[j] = parent2->rules[i].raw[j];
		}
	}
}

void hybridization2(Brain * parent1, Brain * parent2, Brain * child) {
    int rularg1 = nrand() % (P*8);
    int rul1 = rularg1 / 8, arg1 = rularg1 % 8;
    int rularg2 = nrand() % (P*8-1);
    int rul2,arg2;
    //printf("%d,%d\n",rularg1,rularg2);
    if (rularg2 >= rularg1) {
        rularg2++;
        //printf("%d,%d\n",rularg1,rularg2);
        rul2 = rularg2 / 8; arg2 = rularg2 % 8;
        for (int i = 0; i < rul1; i++) {
            for (int j = 0; j < 8; j++) {
                child->rules[i].raw[j] = parent1->rules[i].raw[j];
            }
        }
        if (rul1!=rul2) {
            for (int j = 0; j < arg1; j++) {
                child->rules[rul1].raw[j] = parent1->rules[rul1].raw[j];
            }
            for (int j = arg1; j < 8; j++) {
                child->rules[rul1].raw[j] = parent2->rules[rul1].raw[j];
            }
            for (int i = rul1 + 1; i < rul2; i++) {
                for (int j = 0; j < 8; j++) {
                    child->rules[i].raw[j] = parent2->rules[i].raw[j];
                }
            }
            for (int j = 0; j < arg2; j++) {
                child->rules[rul2].raw[j] = parent2->rules[rul2].raw[j];
            }
            for (int j = arg2; j < 8; j++) {
                child->rules[rul2].raw[j] = parent1->rules[rul2].raw[j];
            }
        }
        else{
            for (int j = 0; j < arg1; j++) {
                child->rules[rul1].raw[j] = parent1->rules[rul1].raw[j];
            }
            for (int j = arg1; j < arg2; j++) {
                child->rules[rul2].raw[j] = parent2->rules[rul2].raw[j];
            }
            for (int j = arg2; j < 8; j++) {
                child->rules[rul1].raw[j] = parent1->rules[rul1].raw[j];
            }
        }
        for (int i = rul2 + 1; i < P; i++) {
            for (int j = 0; j < 8; j++) {
                child->rules[i].raw[j] = parent1->rules[i].raw[j];
            }
        }
    }
    else {
        rul2 = rularg2 / 8; arg2 = rularg2 % 8;
        for (int i = 0; i < rul2; i++) {
            for (int j = 0; j < 8; j++) {
                child->rules[i].raw[j] = parent2->rules[i].raw[j];
            }
        }
        if (rul1!=rul2) {
            for (int j = 0; j < arg2; j++) {
                child->rules[rul2].raw[j] = parent2->rules[rul2].raw[j];
            }
            for (int j = arg2; j < 8; j++) {
                child->rules[rul2].raw[j] = parent1->rules[rul2].raw[j];
            }
            for (int i = rul2 + 1; i < rul1; i++) {
                for (int j = 0; j < 8; j++) {
                    child->rules[i].raw[j] = parent1->rules[i].raw[j];
                }
            }
            for (int j = 0; j < arg1; j++) {
                child->rules[rul1].raw[j] = parent1->rules[rul1].raw[j];
            }
            for (int j = arg1; j < 8; j++) {
                child->rules[rul1].raw[j] = parent2->rules[rul1].raw[j];
            }
        }
        else{
            for (int j = 0; j < arg2; j++) {
                child->rules[rul2].raw[j] = parent2->rules[rul2].raw[j];
            }
            for (int j = arg2; j < arg1; j++) {
                child->rules[rul1].raw[j] = parent1->rules[rul1].raw[j];
            }
            for (int j = arg1; j < 8; j++) {
                child->rules[rul2].raw[j] = parent2->rules[rul2].raw[j];
            }
        }
        for (int i = rul1 + 1; i < P; i++) {
            for (int j = 0; j < 8; j++) {
                child->rules[i].raw[j] = parent2->rules[i].raw[j];
            }
        }
    }
    //printf("rul1 : %d | arg1 : %d | rul2 : %d | arg2 : %d\n",rul1,arg1,rul2,arg2);
    //printBrain(parent1);
    //printBrain(parent2);
    //printBrain(child);
}

void hybridization3(Brain * parent1, Brain * parent2, Brain * parent3, Brain * child) {
    int rul1 = nrand() % (P - 1);
    int rul2 = rul1 + (nrand() % (P - rul1));
    int arg1 = nrand() % 8;
    int arg2 = nrand() % 8;
    for (int i = 0; i < rul1; i++) {
        for (int j = 0; j < 8; j++) {
            child->rules[i].raw[j] = parent1->rules[i].raw[j];
        }
    }
    for (int j = 0; j < arg1; j++) {
        child->rules[rul1].raw[j] = parent1->rules[rul1].raw[j];
    }
    for (int j = arg1 + 1; j < 8; j++) {
        child->rules[rul1].raw[j] = parent2->rules[rul1].raw[j];
    }
    for (int i = rul1 + 1; i < rul2; i++) {
        for (int j = 0; j < 8; j++) {
            child->rules[i].raw[j] = parent2->rules[i].raw[j];
        }
    }
    for (int j = 0; j < arg2; j++) {
        child->rules[rul2].raw[j] = parent2->rules[rul2].raw[j];
    }
    for (int j = arg2 + 1; j < 8; j++) {
        child->rules[rul2].raw[j] = parent3->rules[rul2].raw[j];
    }
    for (int i = rul2 + 1; i < P; i++) {
        for (int j = 0; j < 8; j++) {
            child->rules[i].raw[j] = parent3->rules[i].raw[j];
        }
    }
}

/**
 * @brief save the given brain to ./brains/<level>.<species>
 * @param [in] brain the brain to save
 * @param [in] level the level (= nb of evolution of the brain)
 * @param [in] species the species associate to the brain
 * @param [in] type the type of the AI
 * @return 1 if saved, 0 otherwise
 * */
int save_brain(Brain * brain, int level, Species species, TypeAI type) {
	char str[42];
	int r = 1;
	char * prefix[] = {"xx", "g1", "g2", "ag"};
	mkdir("brains", 0744);
	snprintf(str, 42, "brains/%s.%06d.%s", prefix[type], level, (species == RED) ? "red" : ((species == BLUE) ? "blue" : "green"));
	
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
 * @param [in] level the level (= nb of evolution of the brain) or -1 (last)
 * @param [in] species the species associate to the brain
 * @param [in] type the type of the AI
 * @return the loaded level if loaded, 0 otherwise
 * */
int load_brain(Brain * brain, int level, Species species, TypeAI type) {
	if (level == -1) level = get_last_brain(species, type);
	char str[42];
	int r = 1;
	char * prefix[] = {"xx", "g1", "g2", "ag"};
	
	snprintf(str, 32, "brains/%s.%06d.%s", prefix[type], level, (species == RED) ? "red" : ((species == BLUE) ? "blue" : "green"));
	
	FILE * f = fopen(str, "rb");
	//printf("load>> %p : %s\n", f, str);
	if (f) {
		r &= fread(brain, sizeof(Brain), 1, f) == 1;
		fclose(f);
	} else r = 0;
	return r ? level: 0;
}

/**
 * @brief get the last level of the saved brains of the specified species
 * @param [in] species the species
 * @param [in] type the type of the AI
 * @return the last level or -1
 * */
int get_last_brain(Species species, TypeAI type) {
	char extw[10];
	int lvl, lvl_max = -1;
	char prefix[] = "x12g";
	
	DIR * d = opendir("brains");
	struct dirent * dir;
	
	if (d) {
		snprintf(extw, 10, ".%s", (species == RED) ? "red" : ((species == BLUE) ? "blue" : "green"));
		while ((dir = readdir(d)) != NULL) {
			char * ext;
			if (dir->d_name[1] != prefix[type]) continue;
			lvl = (int) strtol(dir->d_name + 3, &ext, 10);
			if (lvl > lvl_max && strcmp(ext, extw) == 0) lvl_max = lvl; //sscanf(dir->d_name, "%d.%*c", &lvl);
		}
		closedir(d);
	}
	return lvl_max;
}

/**
 * @brief initialize individuals positions to be dispatched and alone in a 3x3 square
 * @param [out] pops the populations
 * @return the same populations
 * */
Populations * init_dispatched_pops(Populations * pops){
	Field field = {.map = {{0}}};

	for (int p = 0; p < 3; p++) for (int i = 0; i < IndividualPerPopulation; i++) {
		Individual * ind = &pops->pops[p].individuals[i];
		int x, y;

		do {
			x = nrand() % SIZEMAP;
			y = nrand() % SIZEMAP;
		} while (field.map[x][y]);
		
		if (x > 0) {
			if (y > 0) field.map[x - 1][y - 1] = 1;
			if (y + 1 < SIZEMAP) field.map[x - 1][y + 1] = 1;
			field.map[x - 1][y] = 1;
		}
		if (x + 1 < SIZEMAP) {
			if (y > 0) field.map[x + 1][y - 1] = 1;
			if (y + 1 < SIZEMAP) field.map[x + 1][y + 1] = 1;
			field.map[x + 1][y] = 1;
		}
		if (y > 0) field.map[x][y - 1] = 1;
		if (y + 1 < SIZEMAP) field.map[x][y + 1] = 1;
		field.map[x][y] = 1;

		ind->x = x;
		ind->y = y;
		ind->alive = 1;
	}

	return pops;
}

/**
 * @brief initialize individuals positions to be grouped
 * @param [out] pops the populations
 * @param [in] offset the offset to select the triangle corner
 * @return the same populations
 * */
Populations * init_grouped_pops(Populations * pops, int offset){
	int hor_b[3] = {SIZEMAP / 3, 2 * SIZEMAP / 3, SIZEMAP / 2},
	    ver_b[3] = {SIZEMAP / 3, SIZEMAP / 3, 2 * SIZEMAP}; // a modif si veut changer position depart
    int hor[3],ver[3];

	for (int p = 0; p < 3; p++) {
        hor[(p + offset) % 3] = hor_b[p];
        ver[(p + offset) % 3] = ver_b[p];
		for (int i = 0; i < IndividualPerPopulation; i++) {
			Individual * ind = &pops->pops[p].individuals[i];
			ind->x = hor[p] + i - IndividualPerPopulation / 2;
			ind->y = ver[p] + i % 2;
			ind->alive = 1;
		}
	}

	return pops;
}

Populations * create_pops(Populations * pops, Brain *brain[3], int decal){
	(void) decal;
    if (!pops) pops = malloc(sizeof(Populations));
    if (pops) {
        pops->iteration = 0;

        init_dispatched_pops(pops);

        for (int i = 0; i < 3; i++) {
            pops->pops[i].species = i + 1;
            pops->pops[i].state.targets = IndividualPerPopulation;
            pops->pops[i].state.alives = IndividualPerPopulation;
            pops->pops[i].state.end_state = 0;
            pops->pops[i].brain = brain[i];
        }
    }
    return pops;
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


#ifdef TESTING2
int main(){
    /*
    snrand(time(NULL));
    Brains * brains = malloc(sizeof(Brains)) ;
    brains->level = 0;
    brains->species = 1;
    brains->brain[0] = rand_brain( NULL);
    for (int k=1; k<BrainPool2 ;k++ ){
        brains->brain[k] = copy_brain(brains->brain[0],NULL);
    }
    int x = mutation_two(brains, nrand()%(8*P));
    for (int k=0; k<x ;k++ ){
        printBrain(brains->brain[k]);
    }*/

    int seed = time(NULL);
    snrand(seed);
    Brains * brains = malloc(sizeof(Brains)) ;
    brains->level = 0;
    brains->species = 1;
    int list[8*P];
    for (int i=0; i<8*P; i++){
        list[i] = i;
    }
    rand_brain( &brains->prey );
    rand_brain( &brains->predator );
    brains->brain[0] = rand_brain( NULL);
    for (int k=1; k<BrainPool2 ;k++ ){
        brains->brain[k] = copy_brain(brains->brain[0],NULL);
    }
    brains->level = 0;
    int x=0;
    for (int evo=0; evo<5; evo++){
        mutation_two_do(brains, list);
        //mutation_all(brains, list, 1);
        if (evo>0) x = nrand()%4;
        if (x%2)  load_brain(&brains->prey, get_last_brain(RED)-1, RED );
        else rand_brain(&brains->prey);
        if (x/2)  load_brain(&brains->predator, get_last_brain(RED)-1, RED );
        else rand_brain(&brains->predator);
        copy_brain(brains->brain[0], &brains->prey );
        copy_brain(brains->brain[0], &brains->predator );
        save_brain(brains->brain[0], evo, brains->species);
        printf("evo : %d\n",evo);
    }
    for (int m=0; m< BrainPool2; m++){
        free(brains->brain[m]);
    }
    free(brains);
    printf("%d\n",seed);
/*    ajouter ailleur pour voir
    int ** field = createField();
    DISTMAXFIELD = sqrt(2) * SIZEMAP;
    fillMatrixFromPops(field, &pops);
    printField(field);
    freeField(field);
*/
}
#endif
