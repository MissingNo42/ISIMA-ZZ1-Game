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

void tournament(Brains  * brains){

}

void change_brains_order(Brains_gen * brains){
    for (int k = NB_BRAINS_CANDIDATE ; k>0; k--){
        int i = rand() % k;
        Brain * tmp = brains->brain[k-1];
        brains->brain[k-1] = brains->brain[i];
        brains->brain[i] = tmp;
    }
}

