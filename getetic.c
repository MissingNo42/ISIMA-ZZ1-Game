//
// Created by lanth on 06/07/2023.
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
#include "getetic.h"

Brains_gen * create_Brains_gen(Brains_gen * brains, Species * species){
    if(!brains) brains = malloc(sizeof(Brains_gen));
    if(brains){
        brains->species = species;
        brains->level = 0;

        for (int i = 0; i < NB_BRAINS_CANDIDATE; i++) {
            rand_brain(brains->brain[i]);
        }
    }
    return brains;
}

void tournament(Brains_gen  * brains){
    change_brains_order(brains);
    rand_brain(&brains->prey);
    rand_brain(&brains->predator);
    Species  species = brains->species;
    for (int k=0; k<NB_BRAINS_CANDIDATE/NB_BRAINS_COMPETING; k++){
        for (int i=0; i<NB_BRAINS_COMPETING; i++){
            int num = k * NB_BRAINS_COMPETING + i;
            Brain *brain_list[3];
            brain_list[species - 1] = brains->brain[num];
            brain_list[species % 3] = &brains->prey;
            brain_list[(species + 1) % 3] = &brains->predator;
            float eval_val = 0;
            for(int anti_rand = 0; anti_rand<9;anti_rand++) {
                Populations *pops = create_pops(NULL, brain_list, anti_rand%3);
                simulate(pops);
                eval(pops, species - 1);
                //printf("ite: %d, eval: %f, alives: %d,targets: %d\n", pops->iteration, pops->pops[species - 1].brain->eval, pops->pops[species - 1].state.alives, pops->pops[species - 1].state.targets);
                eval_val += brains->brain[num]->eval;
                free(pops);
            }
            brains->brain[num]->eval = eval_val / 9;
            printf("\teval : %f\n", brains->brain[num]->eval);
        }
        select_best_gen(brains, k*NB_BRAINS_COMPETING);
    }
    for (int k=0; k < NB_BRAINS_RECOVERED; k++ ){
        int p = NB_BRAINS_CANDIDATE/NB_BRAINS_COMPETING;
        int x = rand() % (NB_BRAINS_CANDIDATE - p - k);
        //Brain * tmp = brains->brain[p + k];
        brains->brain[p + k] =  brains->brain[p + k + x];
        //brains->brain[p + k + x] = tmp;
    }
}

void change_brains_order(Brains_gen * brains){
    for (int k = NB_BRAINS_CANDIDATE ; k>0; k--){
        int i = rand() % k;
        Brain * tmp = brains->brain[k-1];
        brains->brain[k-1] = brains->brain[i];
        brains->brain[i] = tmp;
    }
}

void select_best_gen(Brains_gen * brains, int pos){
    int j = 0;
    float eval = brains->brain[pos]->eval;
    for (int k = 1; k<NB_BRAINS_COMPETING; k++){
        if (eval < brains->brain[pos+k]->eval){
            j = pos+k;
            eval = brains->brain[pos+k]->eval;
        }
    }
    if (j){
        Brain * tmp = brains->brain[j];
        brains->brain[j] = brains->brain[pos/NB_BRAINS_COMPETING];
        brains->brain[pos/NB_BRAINS_COMPETING] = tmp;
    }
}

void reproduction(Brains_gen * brains){
    int nbSelected = NB_BRAINS_CANDIDATE / NB_BRAINS_COMPETING + NB_BRAINS_RECOVERED;
    Brain *tabParents[nbSelected];
    int alreadyChosen[3] = {-1, -1, -1};
    for(int i = 0 ; i < nbSelected ; i++){
        tabParents[i] = brains->brain[i];
    }

    int currentBrains_gen = 0;
    for(int i = 0 ; i < nbSelected ; i++){
        for(int j = 0 ; j < NB_BRAINS_CANDIDATE / nbSelected ; j++){
            int k = rand() % nbSelected;
            while((j >= 0 && k == alreadyChosen[0]) || (j >= 1 && k == alreadyChosen[1]) || (j >= 2 && k == alreadyChosen[2])) {
                k = rand() % nbSelected;
            }
            hybridization(tabParents[i], tabParents[k], brains->brain[currentBrains_gen]);
            currentBrains_gen ++;
            if(j < 3) alreadyChosen[j] = k;
        }
    }
}

int main(){
    srand( time(NULL) );

    Brains_gen * brains = create_Brains_gen(NULL);
    printBrain()

    return 0;
}