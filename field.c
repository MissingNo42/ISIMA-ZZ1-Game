#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#incclude "field.h"
#include "population.h"

float DISTMAXFIELD;

void freeField(int ** field){
    for(int i = 0; i < SIZEMAP; i++){
        if(field[i]) free(field[i]);
    }
    free(field);
}

int ** createField(){
    int ** field = malloc(sizeof(int *) * SIZEMAP);
    if(field) {
        for (int i = 0; i < SIZEMAP; i++) {
            field[i] = calloc(sizeof(int), SIZEMAP);
            if(!field[i]) freeField(field);
        }
    }
    return field;
}

void printField(int ** field){
    for(int i = 0; i < SIZEMAP; i++){
        for(int j = 0; j < SIZEMAP; j++) printf("----");
        printf("-\n|");
        for(int j = 0; j < SIZEMAP; j++){
            if(field[j][i]) printf(" %d |", field[j][i]);
            else printf("   |");
        }
        printf("\n");
    }
    for(int i = 0; i < SIZEMAP; i++) printf("----");
    printf("-\n");
}

void fillMatrixFromPops(int ** field, Populations * pops){
    for(int k = 0; k < 3; k++){
        for(int i = 0; i < IndividualPerPopulation; i++){
            int x = pops->pops[k].individuals[i].x;
            int y = pops->pops[k].individuals[i].y;
            field[x][y] = pops->pops[k].species;
        }
    }
}

// param select : 0 = prey, 1 = predator, 2 = ally
void nearestPPA(Individual * indiv, Population * popPPA, int select){
    int iMin = 0;
    float distMin = DISTMAXFIELD;

    for(int i = 0; i < IndividualPerPopulation; i++){
        if(select != 2 || indiv->x != popPPA->individuals[i].x || indiv->y != popPPA->individuals[i].y) {
            float distTmp = sqrt(
                    pow(indiv->x - popPPA->individuals[i].x, 2) + pow(indiv->y - popPPA->individuals[i].y, 2));
            if (distTmp < distMin) {
                iMin = i;
                distMin = distTmp;
            }
        }
    }

    int xDiff = indiv->x - popPPA->individuals[iMin].x,
    yDiff = indiv->y - popPPA->individuals[iMin].y;

    int DiffPlus = xDiff + yDiff,
    DiffMinus = xDiff - yDiff;
    if (DiffPlus > 0) {

        if (DiffMinus >= 0) indiv->status.infos[select].dir = W;
        else indiv->status.infos[select].dir = N;

    } else if (DiffPlus < 0) {

        if (DiffMinus > 0) indiv->status.infos[select].dir = S;
        else indiv->status.infos[select].dir = E;

    } else {

        if (DiffMinus > 0) indiv->status.infos[select].dir = S;
        else if (DiffMinus < 0) indiv->status.infos[select].dir = N;
        else indiv->status.infos[select].dir = JOKER;
    }

    if(distMin < PERCENTNEAR * DISTMAXFIELD) indiv->status.infos[select].dist = NEAR;
    else if(distMin < PERCENTMEDIUM * DISTMAXFIELD) indiv->status.infos[select].dist = MEDIUM;
    else indiv->status.infos[select].dist = AWAY;
}

void fillStatusPops(Populations * pops){
    for(int k = 0; k < 3; k++){
        for(int i = 0; i < IndividualPerPopulation; i++){
            nearestPPA(&pops->pops[k].individuals[i], &pops->pops[(k+1)%3], 0);
            nearestPPA(&pops->pops[k].individuals[i], &pops->pops[(k+2)%3], 1);
            nearestPPA(&pops->pops[k].individuals[i], &pops->pops[k], 2);
        }
    }
}

int main(){
    int ** field = createField();
    DISTMAXFIELD = sqrt(2) * SIZEMAX;

    Population popRed = {.individuals = {
            {.x = 0, .y = SIZEMAP - 1},
            {.x = 2, .y = SIZEMAP - 1},
            {.x = 4, .y = SIZEMAP - 1},
            {.x = 1, .y = SIZEMAP - 2},
            {.x = 6, .y = SIZEMAP - 1},
            }, NULL, RED};

    Population popGreen = {.individuals = {
            {.x = SIZEMAP - 1, .y = SIZEMAP - 1},
            {.x = SIZEMAP - 3, .y = SIZEMAP - 1},
            {.x = SIZEMAP - 5, .y = SIZEMAP - 1},
            {.x = SIZEMAP - 2, .y = SIZEMAP - 2},
            {.x = SIZEMAP - 4, .y = SIZEMAP - 2},
    }, NULL, GREEN};

    Population popBlue = {.individuals = {
            {.x = 10, .y = 0},
            {.x = 10, .y = 1},
            {.x = 12, .y = 1},
            {.x = 9, .y = 2},
            {.x = 11, .y = 2},
    }, NULL, BLUE};

    Populations pops = {{popRed, popGreen, popBlue}};

    fillMatrixFromPops(field, &pops);

    fillStatusPops(&pops);
    printf("red 5 : [%d, _, %d, _, %d, _]\n", pops.r.individuals[4].status.prey.dir,
           pops.r.individuals[4].status.predator.dir,
           pops.r.individuals[4].status.ally.dir
           );
    printf("green 1 : [%d, _, %d, _, %d, _]\n", pops.g.individuals[0].status.prey.dir,
           pops.g.individuals[0].status.predator.dir,
           pops.g.individuals[0].status.ally.dir
    );
    printf("blue 1 : [%d, _, %d, _, %d, _]\n", pops.b.individuals[0].status.prey.dir,
           pops.b.individuals[0].status.predator.dir,
           pops.b.individuals[0].status.ally.dir
    );

    printField(field);
    freeField(field);
    return 0;
}