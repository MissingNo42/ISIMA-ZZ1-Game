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

}

void change_brains_order(Brains_gen * brains){
    for (int k = NB_BRAINS_CANDIDATE ; k>0; k--){
        int i = rand() % k;
        Brain * tmp = brains->brain[k-1];
        brains->brain[k-1] = brains->brain[i];
        brains->brain[i] = tmp;
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