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


#include "field.h"
#include "getetic.h"

Brains_gen * create_Brains_gen(Brains_gen * brains, Species species){
    if(!brains) brains = malloc(sizeof(Brains_gen));
    if(brains){
        brains->species = species;
        brains->level = 0;

        for (int i = 0; i < NB_BRAINS_CANDIDATE; i++) {
            brains->brain[i] = rand_brain(NULL);
        }
    }
    return brains;
}


void tournament(Brains_gen  * brains){
    change_brains_order(brains);
    rand_brain(&brains->prey);
    rand_brain(&brains->predator);
    Species species = brains->species;
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
            printf("\teval %d : %f\n", num,brains->brain[num]->eval);
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
    int j = pos;
    float eval = brains->brain[pos]->eval;
    for (int k = 1; k<NB_BRAINS_COMPETING; k++){
        if (eval < brains->brain[pos+k]->eval){
            j = pos+k;
            eval = brains->brain[pos+k]->eval;
        }
    }
    Brain * tmp = brains->brain[j];
    brains->brain[j] = brains->brain[pos/NB_BRAINS_COMPETING];
    brains->brain[pos/NB_BRAINS_COMPETING] = tmp;
}


void reproduction(Brains_gen * brains){
    int nbSelected = NB_BRAINS_CANDIDATE / NB_BRAINS_COMPETING + NB_BRAINS_RECOVERED;
    Brain tabParents[nbSelected];
    for(int i = 0 ; i < nbSelected ; i++){
        copy_brain(brains->brain[i], &tabParents[i]);
    }

    int currentBrains_gen = 0;
    for(int i = 0 ; i < nbSelected ; i++){
        int alreadyChosen[3] = {-1, -1, -1};
        for(int j = 0 ; j < NB_BRAINS_CANDIDATE / nbSelected ; j++){
            int k = rand() % nbSelected;
            while((j > 0 && k == alreadyChosen[0]) || (j > 1 && k == alreadyChosen[1]) || (j > 2 && k == alreadyChosen[2])) {
                k = rand() % nbSelected;
            }
            hybridization(&tabParents[i], &tabParents[k], brains->brain[currentBrains_gen]);
            currentBrains_gen ++;
            if(j < 3) alreadyChosen[j] = k;
        }
    }
}

void proba_calculate(float * list, float prob, int nb_max){
    float div = 0;
    for (int k=0; k<nb_max; k++){
        div += powf(prob,-k);
    }
    list[0] = 1/div;
    for (int k=1; k<nb_max; k++){
        list[k] = list[k-1] + powf(prob,-k);
    }
}

void mutate(Brains_gen * brains, float * proba, int nb_max){
    for (int n=0; n<NB_BRAINS_CANDIDATE;n++) {
        float p = (float) rand() / RAND_MAX;
        int k = 0
        while (k < nb_max && p > proba[k]) {
            mutate1(brains->brain[n]);
        }
    }
}

void mutate1(Brain * brain) {
    int ij = rand() % (P*8);
    int i = ij / 8, j = ij % 8;
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

#ifdef TESTING
int main(){
    srand( time(NULL) );

    Brains_gen * brains = create_Brains_gen(NULL, 1);
    tournament(brains);

    return 0;
}
#endif