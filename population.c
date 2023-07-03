//
// Created by lanth on 03/07/2023.
//

#include <math.h>
#include <stdlib.h>

#include "rules.h"


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
            child[i].raw[j] =  parent1[i].raw[j]
        }
    }
    for (int j = 0; j<arg;j++){
        child[rul].raw[j] =  parent1[rul].raw[j]
    }
    for (int j = arg+1; j<8;j++){
        child[rul].raw[j] =  parent2[rul].raw[j]
    }
    for (int i = rul+1; i<P; i++){
        for (int j = 0; j<8;j++){
            child[i].raw[j] =  parent2[i].raw[j]
        }
    }
}