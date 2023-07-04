#ifndef ZZ1GAME_FIELD_H
#define ZZ1GAME_FIELD_H

#include "population.h"

#define SIZEMAP 15
#define PERCENTNEAR 0.2
#define PERCENTMEDIUM 0.5

extern float DISTMAXFIELD;

void freeField(int ** field);
int ** createField();
void printField(int ** field);
void fillMatrixFromPops(int ** field, Populations * pops);
void nearestPPA(int indexIndiv, Populations * pops, int k, int select);
void fillStatusPops(Populations * pops);
void printStatus(Populations * pops, Species color, int IndiceIndiv);
void cleanMatrixFromPops(int ** field, Populations * pops);

#endif //ZZ1GAME_FIELD_H
