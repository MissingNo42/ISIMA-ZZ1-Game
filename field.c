#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "field.h"
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
void nearestPPA(int indexIndiv, Populations * pops, int k, int select){
    int iMin = 0;
    float distMin = DISTMAXFIELD;
    Individual * indiv = &pops->pops[k].individuals[indexIndiv];
    Population * popPPA = &pops->pops[(k + select + 1)%3];

    for(int i = 0; i < IndividualPerPopulation; i++){
        if(select != 2 || i != indexIndiv) {
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
            nearestPPA(i, pops, k, 0);
            nearestPPA(i, pops, k, 1);
            nearestPPA(i, pops, k, 2);
        }
    }
}

//Ne pas regardez, c'est une horreur
void printStatus(Populations * pops, Species color, int IndiceIndiv){
    printf("%s ", (color == RED) ? "Red" : ((color == GREEN) ? "Green" : "Blue"));
    color --;
    printf("%d : [%s = direction proie plus proche,\n%s = distance proie plus proche,\n%s = direction predateur plus proche,\n%s = distance predateur plus proche,\n%s = direction allié plus proche,\n%s = distance allié plus proche,\n]\n",
           IndiceIndiv,
           (pops->pops[color].individuals[IndiceIndiv].status.prey.dir == -1) ? "JOKER" : ((pops->pops[color].individuals[IndiceIndiv].status.prey.dir == 0) ? "N" : ((pops->pops[color].individuals[IndiceIndiv].status.prey.dir == 1) ? "E" : ((pops->pops[color].individuals[IndiceIndiv].status.prey.dir == 2) ? "S" : "W"))),
           (pops->pops[color].individuals[IndiceIndiv].status.prey.dist == -1) ? "ANY" : ((pops->pops[color].individuals[IndiceIndiv].status.prey.dist == 0) ? "AWAY" : ((pops->pops[color].individuals[IndiceIndiv].status.prey.dist == 1) ? "MEDIUM" : "NEAR")),
           (pops->pops[color].individuals[IndiceIndiv].status.predator.dir == -1) ? "JOKER" : ((pops->pops[color].individuals[IndiceIndiv].status.predator.dir == 0) ? "N" : ((pops->pops[color].individuals[IndiceIndiv].status.predator.dir == 1) ? "E" : ((pops->pops[color].individuals[IndiceIndiv].status.predator.dir == 2) ? "S" : "W"))),
           (pops->pops[color].individuals[IndiceIndiv].status.predator.dist == -1) ? "ANY" : ((pops->pops[color].individuals[IndiceIndiv].status.predator.dist == 0) ? "AWAY" : ((pops->pops[color].individuals[IndiceIndiv].status.predator.dist == 1) ? "MEDIUM" : "NEAR")),
           (pops->pops[color].individuals[IndiceIndiv].status.ally.dir == -1) ? "JOKER" : ((pops->pops[color].individuals[IndiceIndiv].status.ally.dir == 0) ? "N" : ((pops->pops[color].individuals[IndiceIndiv].status.ally.dir == 1) ? "E" : ((pops->pops[color].individuals[IndiceIndiv].status.ally.dir == 2) ? "S" : "W"))),
           (pops->pops[color].individuals[IndiceIndiv].status.ally.dist == -1) ? "ANY" : ((pops->pops[color].individuals[IndiceIndiv].status.ally.dist == 0) ? "AWAY" : ((pops->pops[color].individuals[IndiceIndiv].status.ally.dist == 1) ? "MEDIUM" : "NEAR"))
    );
}

int main2(){
    int ** field = createField();
    DISTMAXFIELD = sqrt(2) * SIZEMAP;

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
    printStatus(&pops, RED, 4);
    printStatus(&pops, GREEN, 0);
    printStatus(&pops, BLUE, 0);

    printField(field);
    freeField(field);
    return 0;
}