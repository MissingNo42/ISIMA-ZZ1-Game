//
// Created by Romain on 05/07/2023.
//
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include "nrand.h"

#include "population.h"
#include "getetic.h"

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
		int ch1, ch2;
		if (opp) {
			if (randm) {
				ch1 = nrand() % (evo < randm ? evo: randm),
				ch2 = nrand() % (evo < randm ? evo: randm);
				
				if (!ch1) rand_brain(&brains.prey);
				else {
					if (!load_brain(&brains.prey, evo - ch1, brains.species, Glouton1)){
						printf("ERROR load prey : %d %d\n", ch1, evo - ch1);
					}
					ch1 = evo - ch1;
				}

				if (!ch2) rand_brain(&brains.predator);
				else {
					if (!load_brain(&brains.predator, evo - ch2, brains.species, Glouton1)){
						printf("ERROR load predator : %d %d\n", ch2, evo - ch2);
					}
					ch2 = evo - ch2;
				}
			} else if (evo - opp >= 1) {
				ch1 = ch2 = evo - opp;
				load_brain(&brains.prey, ch1, brains.species, Glouton1);
	            copy_brain(&brains.prey, &brains.predator);
			} else goto randai;
		} else {
			randai:
			ch1 = ch2 = 0;
			rand_brain( &brains.prey);
			rand_brain(&brains.predator);
		}
		
		printf("iter %d (gen %d) : prey = %d | predator = %d\n", evo - level, evo, ch1, ch2);
        mutation_all(&brains, list, 1);
		
        save_brain(brains.brain[0], evo, brains.species, Glouton1);
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
		int ch1, ch2;
		if (opp) {
			if (randm) {
				ch1 = nrand() % (evo < randm ? evo: randm),
				ch2 = nrand() % (evo < randm ? evo: randm);
				
				if (!ch1) rand_brain(&brains.prey);
				else {
					if (!load_brain(&brains.prey, evo - ch1, brains.species, Glouton2)){
						printf("ERROR load prey : %d %d\n", ch1, evo - ch1);
					}
					ch1 = evo - ch1;
				}

				if (!ch2) rand_brain(&brains.predator);
				else {
					if (!load_brain(&brains.predator, evo - ch2, brains.species, Glouton2)){
						printf("ERROR load predator : %d %d\n", ch2, evo - ch2);
					}
					ch2 = evo - ch2;
				}
				
			} else if (evo - opp >= 1) {
				ch1 = ch2 = evo - opp;
				load_brain(&brains.prey, ch1, brains.species, Glouton2);
	            copy_brain(&brains.prey, &brains.predator);
			} else goto randai;
		} else {
			randai:
			ch1 = ch2 = 0;
			rand_brain( &brains.prey);
			rand_brain(&brains.predator);
		}
		
		printf("iter %d (gen %d) : prey = %d | predator = %d\n", evo - level, evo, ch1, ch2);
        mutation_two_do(&brains, list);
		
        save_brain(brains.brain[0], evo, brains.species, Glouton2);
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

void algoG(int color, int iter, int opp){
    Brains_gen brains;
    create_Brains_gen(&brains, color);
    float proba[3];
    proba_calculate(proba, 100, 3);
	
	Brain Att, Def;
	printf("Load Att AI : %d", load_brain(&Att, 10000, RED, 0));
	printf("Load Def AI : %d", load_brain(&Def, 10000, BLUE, 0));

    for(int evo = 1; evo <= iter; evo++){
		printf("iter %d\n", evo);
		
		if (opp > nrand() % 101) {
			copy_brain(&Att, &brains.prey);
			copy_brain(&Def, &brains.predator);
		} else {
		    rand_brain(&brains.prey);
		    rand_brain(&brains.predator);
		}

        Brain * b = tournament(&brains);

        save_brain(b, evo, color, AlgoG);

        reproduction(&brains);

        mutate(&brains, proba, 3);
    }
	printf("end\n");
	
	ask("\nItereration wanted\nN - 'n' iterations\n\nSelect : ", 9999999);
	
	
    for (int m=0; m< NB_BRAINS_CANDIDATE; m++){
        free(brains.brain[m]);
    }
}



#ifdef TRAINING
int main(int argc, char ** argv){
	
	int c = 0, a = 0;
	long int seed = 0;
	while (1) {
		c = ask("Seed = %ld\n\nMENU\n0 - Exit\n1 - Training Glouton-1\n2 - Training Glouton-2\n9 - Set Seed\n\nSelect : ", 9, seed);
		
		snrand(seed);
		switch(c) {
			case 0: exit(0);
			case 1: {
				int color = ask("\nSpecies\n0 - Red\n1 - Green\n2 - blue\n\nSelect : ", 2);
				int max = get_last_brain(color + 1, Glouton1);
				int level = ask("\nStarting Level\n0 - New\n1 - Last = %d\nN - existing level 'n'\n\nSelect : ", (max < 0) ? 0: max, max);
				int iter = ask("\nItereration wanted\nN - 'n' iterations\n\nSelect : ", 9999);
				int opp = ask("\nOpponent Level\n0 - Random\n1 - Both: ...\nN - Use the current training AI : 'level - n'\n\nSelect : ", 9999);
				if (opp == 1) opp = -ask("\nOpponent random alea Level\nN - Use the current training AI : 'level - n'\n\nSelect : ", 9999);
				glouton1(color + 1, level, iter, opp);
				break;
			}
			case 2: {
				int color = ask("\nSpecies\n0 - Red\n1 - Green\n2 - blue\n\nSelect : ", 2);
				int max = get_last_brain(color + 1, Glouton2);
				int level = ask("\nStarting Level\n0 - New\n1 - Last = %d\nN - existing level 'n'\n\nSelect : ", (max < 0) ? 0: max, max);
				int iter = ask("\nItereration wanted\nN - 'n' iterations\n\nSelect : ", 9999);
				int opp = ask("\nOpponent Level\n0 - Random\n1 - Both ...\nN - Use the current training AI : 'level - n'\n\nSelect : ", 9999);
				if (opp == 1) opp = -ask("\nOpponent random alea Level\nN - Use the current training AI : 'level - n'\n\nSelect : ", 9999);
				glouton2(color + 1, level, iter, opp);
				break;
			}
			case 4: {
				int color = ask("\nSpecies\n0 - Red\n1 - Green\n2 - blue\n\nSelect : ", 2);
				int iter = ask("\nItereration wanted\nN - 'n' iterations\n\nSelect : ", 9999999);
				int opp = ask("\nOpponent Type\n0 - Random\nN - Both (N% agressive) \n100 - Full agressive\n\nSelect : ", 100);
				algoG(color + 1, iter, opp);
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
						do {
							printf("New seed : ");
						} while(!scanf("%ld", &seed));
						
						break;
				}
				break;
			}
		}
	}
}

#endif
