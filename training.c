//
// Created by Romain on 05/07/2023.
//
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>

#include "population.h"

void glouton1(int color, int level, int iter, int opp){
	Brains brains;
    brains.level = 0;
    brains.species = color;
	int list[8*P];
    for (int i=0; i<8*P; i++) list[i] = i;
	
    rand_brain( &brains.prey );
    rand_brain( &brains.predator );
	if (level == 0) brains.brain[0] = rand_brain( NULL);
	else {
		brains.brain[0] = malloc(sizeof(Brain));
		if (level == 1) level = get_last_brain(color);
		load_brain(brains.brain[0], level, color);
	}
    for (int k=1; k<BrainPool2 ;k++ ){
        brains.brain[k] = copy_brain(brains.brain[0],NULL);
    }
		
	for (int evo = 1 + level; evo <= iter + level; evo++){
		printf("iter %d (gen %d)\n", evo - level, evo);
        mutation_all(&brains, list, 1);
		if (opp) {
			if (evo - opp >= 1) {
				load_brain(&brains.prey, evo - opp, brains.species);
	            copy_brain(&brains.prey, &brains.predator);
			}
		} else {
			rand_brain( &brains.prey);
			rand_brain(&brains.predator);
		}
        save_brain(brains.brain[0], evo, brains.species);
        printf("evo : %d\n",evo);
    }
	printf("end\n");
    for (int m=0; m< BrainPool2; m++){
        free(brains.brain[m]);
    }
}

void glouton2(int color, int level, int iter, int opp){
	Brains * brains = malloc(sizeof(Brains));
    brains->level = 0;
    brains->species = color;
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
    Species species = brains->species;
	
	for (int evo=0; evo<5; evo++){
        int nb = 0;//mutation_two_do(brains, list);
        printf("\t\tmut2 :");
        for (int num=0; num<nb; num++){
            Brain *brain_list[3];
            brain_list[species - 1] = brains->brain[num];
            brain_list[species % 3] = &brains->prey;
            brain_list[(species + 1) % 3] = &brains->predator;
            float eval_val = 0;
            for(int anti_rand = 0; anti_rand<9;anti_rand++) {
                Populations *pops = create_pops(NULL, brain_list, anti_rand%3);
                simulate(pops);
                eval(pops, species - 1);printf("ite: %d, eval: %f, alives: %d,targets: %d\n", pops->iteration, pops->pops[species - 1].brain->eval, pops->pops[species - 1].state.alives, pops->pops[species - 1].state.targets);
                eval_val += brains->brain[num]->eval;
                free(pops);
            }
            brains->brain[num]->eval = eval_val / 9;
            printf("\teval : %f\n", brains->brain[num]->eval);
        }
        select_best(brains,nb);
        printf("\t\tmutall :");
        mutation_all(brains, list, 1);
        copy_brain(brains->brain[0], &brains->prey );
        copy_brain(brains->brain[0], &brains->predator );
        save_brain(brains->brain[0], evo, brains->species);
        printf("evo : %d\n",evo);
    }
    for (int m=0; m< BrainPool2; m++){
        free(brains->brain[m]);
    }
}

int ask(char * str, int limit, ...) {
	va_list l;
	int r = -1;
	do {
		va_start(l, limit);
		vprintf(str, l);
		va_end(l);
	} while (!scanf("%d", &r) || r < 0 || r > limit);
	return r;
}

#ifdef TRAINING
int main(int argc, char ** argv){
	
	int c = 0, a = 0;
	long int seed = 0;
	while (1) {
		c = ask("Seed = %ld\n\nMENU\n0 - Exit\n1 - Training Glouton-1\n2 - Training Glouton-2\n9 - Set Seed\n\nSelect : ", 9, seed);
		
		srand(seed);
		switch(c) {
			case 0: exit(0);
			case 1: {
				int color = ask("\nSpecies\n0 - Red\n1 - Green\n2 - blue\n\nSelect : ", 2);
				int max = get_last_brain(color + 1);
				int level = ask("\nStarting Level\n0 - New\n1 - Last = %d\nN - existing level 'n'\n\nSelect : ", (max < 0) ? 0: max, max);
				int iter = ask("\nItereration wanted\nN - 'n' iterations\n\nSelect : ", 9999);
				int opp = ask("\nOpponent Level\n0 - Random\nN - Use the current training AI : 'level - n'\n\nSelect : ", 10);
				glouton1(color + 1, level, iter, opp);
				break;
			}
			case 2: {
				int color = ask("\nSpecies\n0 - Red\n1 - Green\n2 - blue\n\nSelect : ", 2);
				int max = get_last_brain(color + 1);
				int level = ask("\nStarting Level\n0 - New\n1 - Last = %d\nN - existing level 'n'\n\nSelect : ", (max < 0) ? 0: max, max);
				int iter = ask("\nItereration wanted\nN - 'n' iterations\n\nSelect : ", 9999);
				int opp = ask("\nOpponent Level\n0 - Random\nN - Use the current training AI : 'level - n'\n\nSelect : ", 9999);
				glouton2(color + 1, level, iter, opp);
				break;
			}
			case 4: {
				printf("Algo G - WIP\n");
				break;
			}
			case 9: {
				do {
					printf("0 - Quit\n1 - Random\n2 - Manual\n\nSelect : ");
				} while (!scanf("%d", &a));
				switch(a) {
					case 0: break;
					case 1:
						seed = time(NULL);
						break;
					case 2:
						scanf("%ld", &seed);
						break;
				}
				break;
			}
		}
	}
}

#endif
