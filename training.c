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
	
	int randm = 0;
	
	if (opp < 0) {
		opp = -opp;
		randm = opp + 1;
	}
	
	int list[8*P];
    for (int i=0; i<8*P; i++) list[i] = i;
	
    rand_brain( &brains.prey );
    rand_brain( &brains.predator );
	if (level == 0) brains.brain[0] = rand_brain( NULL);
	else {
		brains.brain[0] = malloc(sizeof(Brain));
		if (level == 1) level = get_last_brain(color, Glouton1);
		load_brain(brains.brain[0], level, color, Glouton1);
	}
    for (int k=1; k<BrainPool2 ;k++){
        brains.brain[k] = copy_brain(brains.brain[0],NULL);
    }
		
	for (int evo = 1 + level; evo <= iter + level; evo++){
		printf("iter %d (gen %d)\n", evo - level, evo);
        mutation_all(&brains, list, 1);
		if (opp) {
			if (randm) {
				int ch = rand() % (evo < randm ? evo: randm);
				if (!ch) rand_brain(&brains.prey);
				else load_brain(&brains.prey, evo - ch, brains.species, Glouton1);

				ch = rand() % (evo < randm ? evo: randm);
				if (!ch) rand_brain(&brains.predator);
				else load_brain(&brains.predator, evo - ch, brains.species, Glouton1);
				
			} else if (evo - opp >= 1) {
				load_brain(&brains.prey, evo - opp, brains.species, Glouton1);
	            copy_brain(&brains.prey, &brains.predator);
			} else goto randai;
		} else {
			randai:
			rand_brain( &brains.prey);
			rand_brain(&brains.predator);
		}
        save_brain(brains.brain[0], evo, brains.species, Glouton1);
        printf("evo : %d\n",evo);
    }
	printf("end\n");
    for (int m=0; m< BrainPool2; m++){
        free(brains.brain[m]);
    }
}

void glouton2(int color, int level, int iter, int opp){
	Brains brains;
    brains.level = 0;
    brains.species = color;
	
	int randm = 0;
	
	if (opp < 0) {
		opp = -opp;
		randm = opp + 1;
	}
	
	int list[8*P];
    for (int i=0; i<8*P; i++) list[i] = i;
	
    rand_brain( &brains.prey );
    rand_brain( &brains.predator );
	if (level == 0) brains.brain[0] = rand_brain( NULL);
	else {
		brains.brain[0] = malloc(sizeof(Brain));
		if (level == 1) level = get_last_brain(color, Glouton2);
		load_brain(brains.brain[0], level, color, Glouton2);
	}
    for (int k=1; k<BrainPool2 ;k++ ){
        brains.brain[k] = copy_brain(brains.brain[0],NULL);
    }
		
	for (int evo = 1 + level; evo <= iter + level; evo++){
		printf("iter %d (gen %d)\n", evo - level, evo);
        mutation_two_do(&brains, list);
		if (opp) {
			if (randm) {
				int ch = rand() % (evo < randm ? evo: randm);
				if (!ch) rand_brain(&brains.prey);
				else load_brain(&brains.prey, evo - ch, brains.species, Glouton2);

				ch = rand() % (evo < randm ? evo: randm);
				if (!ch) rand_brain(&brains.predator);
				else load_brain(&brains.predator, evo - ch, brains.species, Glouton2);
				
			} else if (evo - opp >= 1) {
				load_brain(&brains.prey, evo - opp, brains.species, Glouton2);
	            copy_brain(&brains.prey, &brains.predator);
			} else goto randai;
		} else {
			randai:
			rand_brain( &brains.prey);
			rand_brain(&brains.predator);
		}
        save_brain(brains.brain[0], evo, brains.species, Glouton2);
        printf("evo : %d\n",evo);
    }
	printf("end\n");
    for (int m=0; m< BrainPool2; m++){
        free(brains.brain[m]);
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
				int opp = ask("\nOpponent Level\n0 - Random\n1 - Both: 50% each\nN - Use the current training AI : 'level - n'\n\nSelect : ", 9999);
				if (opp == 1) opp = -ask("\nOpponent random alea Level\nN - Use the current training AI : 'level - n'\n\nSelect : ", 9999);
				glouton1(color + 1, level, iter, opp);
				break;
			}
			case 2: {
				int color = ask("\nSpecies\n0 - Red\n1 - Green\n2 - blue\n\nSelect : ", 2);
				int max = get_last_brain(color + 1);
				int level = ask("\nStarting Level\n0 - New\n1 - Last = %d\nN - existing level 'n'\n\nSelect : ", (max < 0) ? 0: max, max);
				int iter = ask("\nItereration wanted\nN - 'n' iterations\n\nSelect : ", 9999);
				int opp = ask("\nOpponent Level\n0 - Random\n1 - Both: 50% each\nN - Use the current training AI : 'level - n'\n\nSelect : ", 9999);
				if (opp == 1) opp = -ask("\nOpponent random alea Level\nN - Use the current training AI : 'level - n'\n\nSelect : ", 9999);
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