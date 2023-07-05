#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>

#include "field.h"
#include "population.h"

float DISTMAXFIELD = 1.4142 * SIZEMAP;


void printField(Field * field){
    for(int i = 0; i < SIZEMAP; i++){
        for(int j = 0; j < SIZEMAP; j++) printf("----");
        printf("-\n|");
        for(int j = 0; j < SIZEMAP; j++){
            if(field->map[j][i]) printf(" %d |", field->map[j][i]);
            else printf("   |");
        }
        printf("\n");
    }
    for(int i = 0; i < SIZEMAP; i++) printf("----");
    printf("-\n");
}

void cleanMatrixFromPops(Field * field, Populations * pops){
    for(int k = 0; k < 3; k++){
        for(int i = 0; i < IndividualPerPopulation; i++){
            int x = pops->pops[k].individuals[i].x;
            int y = pops->pops[k].individuals[i].y;
            field->map[x][y] = 0;
        }
    }
}

void fillMatrixFromPops(Field * field, Populations * pops){
    for(int k = 0; k < 3; k++){
        for(int i = 0; i < IndividualPerPopulation; i++){
			if (pops->pops[k].individuals[i].alive) {
				int x = pops->pops[k].individuals[i].x;
				int y = pops->pops[k].individuals[i].y;
				field->map[x][y] = pops->pops[k].species;
			}
        }
    }
}

void update_status(Populations * pops){
    int nearest = 0;
    float dm = FLT_MAX;
	int Dx, Dy;

	for(int p = 0; p < 3; p++){
        for(int i = 0; i < IndividualPerPopulation; i++) {
		    Individual * self = &pops->pops[p].individuals[i];
			if (!self->alive) continue;

			for(int q = 0; q < 3; q++){

				dm = FLT_MAX;
				nearest = -1;

		        for(int u = 0; u < IndividualPerPopulation; u++) { // Search the nearest

					Individual * alt = &pops->pops[q].individuals[u];
					if (!alt->alive || (i == u && q == p)) continue;

					int dx = self->x - alt->x,
					    dy = self->y - alt->y;
					float dt = dx * dx + dy * dy;

					if (dt < dm) { // upd the nearest
						dm = dt;
						nearest = u;
						Dx = dx, Dy = dy;
					}
				}
				Info * info = &self->status.infos[(q - p + 2) % 3];

				if (nearest == -1){
					info->dir = -1;
					info->dist = -1;
				} else {
					int dplus = Dx + Dy,
							dmin = Dx - Dy;

					if (dplus > 0) info->dir = (dmin >= 0) ? W : N;
					else if (dplus < 0) info->dir = (dmin > 0) ? S : E;
					else info->dir = (dmin > 0) ? S : ((dmin < 0) ? N : JOKER);
					
					if (dm < CELLNEAR * CELLNEAR) info->dist = NEAR; // 2 * PC² * SIZE²
					else if (dm < CELLMEDIUM * CELLMEDIUM) info->dist = MEDIUM;
					else info->dist = AWAY;
				}
			}
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
		if (popPPA->individuals[i].alive) {
			if (select != 2 || i != indexIndiv) {
				float distTmp = sqrt(
						pow(indiv->x - popPPA->individuals[i].x, 2) + pow(indiv->y - popPPA->individuals[i].y, 2));
				if (distTmp < distMin) {
					iMin = i;
					distMin = distTmp;
				}
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

	//indiv->status.raw[select*2] = iMin;
	//indiv->status.raw[select*2+1] = (k + select + 1)%3;
}

void fillStatusPops(Populations * pops){
    for(int k = 0; k < 3; k++){
        for(int i = 0; i < IndividualPerPopulation; i++){
			if (!pops->pops[k].individuals[i].alive) continue;
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
/*
#ifdef TESTING
int main(){
	srand(1688482972);
	Field field = {.map = {{0}}};
    DISTMAXFIELD = sqrt(2) * SIZEMAP;
	
	Brain b[3] = {{.eval = 0}, {.eval = 0}, {.eval = 0}};
	
	rand_brain(b + 0);
	rand_brain(b + 1);
	rand_brain(b + 2);

    Population popRed = {.individuals = {
            {.x = 0, .y = SIZEMAP - 1, .alive = 1},
            {.x = 2, .y = SIZEMAP - 1, .alive = 1},
            {.x = 4, .y = SIZEMAP - 1, .alive = 1},
            {.x = 1, .y = SIZEMAP - 2, .alive = 1},
            {.x = 6, .y = SIZEMAP - 1, .alive = 1},
            }, b+ 0, RED, .state = {.end_state = None, .alives = IndividualPerPopulation, .targets = IndividualPerPopulation}};

    Population popGreen = {.individuals = {
            {.x = SIZEMAP - 1, .y = SIZEMAP - 1, .alive = 1},
            {.x = SIZEMAP - 3, .y = SIZEMAP - 1, .alive = 1},
            {.x = SIZEMAP - 5, .y = SIZEMAP - 1, .alive = 1},
            {.x = SIZEMAP - 2, .y = SIZEMAP - 2, .alive = 1},
            {.x = SIZEMAP - 4, .y = SIZEMAP - 2, .alive = 1},
    }, b+1, GREEN, .state = {.end_state = None, .alives = IndividualPerPopulation, .targets = IndividualPerPopulation}};

    Population popBlue = {.individuals = {
            {.x = 10, .y = 0, .alive = 1},
            {.x = 10, .y = 1, .alive = 1},
            {.x = 12, .y = 1, .alive = 1},
            {.x = 9, .y = 2, .alive = 1},
            {.x = 11, .y = 2, .alive = 1},
    }, b+2, BLUE, .state = {.end_state = None, .alives = IndividualPerPopulation, .targets = IndividualPerPopulation}};

    Populations pops = {.pops = {popRed, popGreen, popBlue}, .iteration = 0};

    fillMatrixFromPops(&field, &pops);

    printStatus(&pops, RED, 4);
    printStatus(&pops, GREEN, 0);
    printStatus(&pops, BLUE, 0);
	int s=5;
    //printField(field);
	for (int i = 0; !is_terminated(&pops); i++){
		printField(&field);
		cleanMatrixFromPops(&field, &pops);

        update_status(&pops);
		move(&pops);
		printf(">> Terminated: %d : %d\n", is_terminated(&pops), i);
		//exit(1);
	    fillMatrixFromPops(&field, &pops);

	    printField(&field);
	}
    return 0;
}
#endif*/
