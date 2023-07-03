//
// Created by lanth on 03/07/2023.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include "population.h"
#include "rules.h"

void predict_move(Population * pop){
	for (int i = 0; i < IndividualPerPopulation; i++){
		int ch = choice_rule(&pop->individuals[i].status, *pop->brain);
		pop->individuals[i].action = (ch == -1) ? JOKER: (*pop->brain)[ch].action;
	}
}

void execute_move (Populations * populations ){
    for (int pop = 0; pop < 3; pop++) {
        for (int i = 0; i < IndividualPerPopulation; i++) {
            Individual  * id = &(populations->pops[pop].individuals[i]);
            if (id->alive) {
                if (id->action == JOKER) {
                    id->action = rand() % 4;
                }
                switch (id->action){
                    case N:
                        if (id->y - 1 < 0) id->y -=1;
                    case E:
                        if (id->x + 1 > SIZEMAP) id->x +=1;
                    case S:
                        if (id->y + 1 > SIZEMAP) id->y +=1;
                    case W:
                        if (id->x - 1 < 0) id->x -=1;

                }
            }
        }
    }
}

void mutation (Brain  brain){
    int i = rand() % P;
    int j = rand() % 8;
    int k = rand() ;
    int d = 0;
    if (k > 0.1 * RAND_MAX){
        if (k > 0.55 * RAND_MAX) d = 1;
        else d = -1;
    }
    switch (j){
        case 0:
            if (d == 0) brain[i].prey.dir = -1;
            else brain[i].prey.dir = (brain[i].prey.dir + d + 4) % 4;
            break;
        case 1:
            if (d == 0) brain[i].prey.dist = -1;
            else brain[i].prey.dist = (brain[i].prey.dist + d + 3) % 3;
            break;
        case 2:
            if (d == 0) brain[i].predator.dir = -1;
            else brain[i].predator.dir = (brain[i].predator.dir + d + 4) % 4;
            break;
        case 3:
            if (d == 0) brain[i].predator.dist = -1;
            else brain[i].predator.dist = (brain[i].predator.dist + d + 3) % 3;
            break;
        case 4:
            if (d == 0) brain[i].ally.dir = -1;
            else brain[i].ally.dir = (brain[i].ally.dir + d + 4) % 4;
            break;
        case 5:
            if (d == 0) brain[i].ally.dist = -1;
            else brain[i].ally.dist = (brain[i].ally.dist + d + 3) % 3;
            break;
        case 6:
            if (d==0) brain[i].action = -1;
            else brain[i].action = (brain[i].action + d + 4) % 4;
            break;
        case 7:
            brain[i].priority = (brain[i].priority + d + MAX_PRIORITY) % MAX_PRIORITY;
            break;
    }
}

void hybridization (Brain parent1, Brain parent2,Brain child){
    int rul = rand() % P;
    int arg = rand() % 8;
    for (int i = 0; i<rul; i++){
        for (int j = 0; j<8;j++){
            child[i].raw[j] =  parent1[i].raw[j];
        }
    }
    for (int j = 0; j<arg;j++){
        child[rul].raw[j] =  parent1[rul].raw[j];
    }
    for (int j = arg+1; j<8;j++){
        child[rul].raw[j] =  parent2[rul].raw[j];
    }
    for (int i = rul+1; i<P; i++){
        for (int j = 0; j<8;j++){
            child[i].raw[j] =  parent2[i].raw[j];
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
int save_brain(Brain brain, int level, Species species) {
	char str[42];
	int r = 1;
	
	mkdir("brains", 0744);
	snprintf(str, 32, "brains/%06d.%s", level, (species == RED) ? "red": ((species == BLUE) ? "blue": "green"));
	
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
int load_brain(Brain brain, int level, Species species) {
	char str[42];
	int r = 1;
	
	snprintf(str, 32, "brains/%06d.%s", level, (species == RED) ? "red": ((species == BLUE) ? "blue": "green"));
	
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
	
	DIR *d = opendir("brains");
	struct dirent *dir;
	
	if (d) {
		snprintf(extw, 10, ".%s", (species == RED) ? "red": ((species == BLUE) ? "blue": "green"));
		while ((dir = readdir(d)) != NULL) {
			char * ext;
			lvl = (int)strtol(dir->d_name, &ext, 10);
			if (lvl > lvl_max && strcmp(ext, extw) == 0) lvl_max = lvl; //sscanf(dir->d_name, "%d.%*c", &lvl);
		}
		closedir(d);
	}
	return lvl_max;
}

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
