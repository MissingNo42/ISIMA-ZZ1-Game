#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "population.h"

#define SIZEMAP 15
#define PERCENTNEAR 0.2
#define PERCENTMEDIUM 0.5

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

// param select : 0 = prey, 1 = predator, 2 = ally
void nearestPrey(Individual * indiv, Population popPrey, int select){
    int iMin = 0;
    float distMin = sqrt(pow(indiv->x - popPrey.individuals[0].x, 2) + pow(indiv->y - popPrey.individuals[0].y, 2));

    for(int i = 1; i < IndividualPerPopulation; i++){
        float distTmp = sqrt(pow(indiv->x - popPrey.individuals[i].x, 2) + pow(indiv->y - popPrey.individuals[i].y, 2));
        if(distTmp < distMin){
            iMin = i;
            distMin = distTmp;
        }
    }

    int xDiff = indiv->x - popPrey.individuals[iMin].x,
    yDiff = indiv->y - popPrey.individuals[iMin].y;

    if (xDiff + yDiff > 0) {
        if (xDiff - yDiff >= 0) indiv->status.infos[select].dir = W;
        else indiv->status.infos[select].dir = N;
    } else if (xDiff + yDiff < 0) {
        if (xDiff - yDiff > 0) indiv->status.infos[select].dir = S;
        else indiv->status.infos[select].dir = E;
    } else {
        if (xDiff - yDiff > 0) indiv->status.infos[select].dir = S;
        else if (xDiff - yDiff < 0) indiv->status.infos[select].dir = N;
        else indiv->status.infos[select].dir = JOKER;
    }

    if(distMin < PERCENTNEAR * sqrt(2) * SIZEMAP) indiv->status.infos[select].dist = NEAR;
    else if(distMin < PERCENTMEDIUM * sqrt(2) * SIZEMAP) indiv->status.infos[select].dist = MEDIUM;
    else indiv->status.infos[select].dist = AWAY;
}

int main(){
    int ** field = createField();

    Population popPrey = {.individuals = {
            {.x = 0, .y = 0},
            {.x = 0, .y = SIZEMAP - 1},
            {.x = SIZEMAP - 1, .y = 0},
            {.x = 4, .y = 13},
            {.x = 12, .y = 5},
            }, NULL, GREEN};

    field[0][0] = 2;
    field[0][SIZEMAP - 1] = 2;
    field[SIZEMAP - 1][0] = 2;
    field[4][13] = 2;
    field[12][5] = 2;

    Individual indiv = {.x = 8, .y = 4};
    field[8][4] = 1;

    printf("%d, %d\n", indiv.status.prey.dir, indiv.status.prey.dist);
    nearestPrey(&indiv, popPrey);
    printf("%d, %d\n", indiv.status.prey.dir, indiv.status.prey.dist);

    printField(field);
    freeField(field);
    return 0;
}