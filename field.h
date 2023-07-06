#ifndef ZZ1GAME_FIELD_H
#define ZZ1GAME_FIELD_H

#include "population.h"

#define PERCENTNEAR 0.2
#define PERCENTMEDIUM 0.5
#define CELLNEAR 3
#define CELLMEDIUM 10

extern float DISTMAXFIELD;




void printField(Field * field);
void fillMatrixFromPops(Populations * pops);
void nearestPPA(int indexIndiv, Populations * pops, int k, int select);
void update_status(Populations * pops);
void printStatus(Populations * pops, Species color, int IndiceIndiv);
void cleanMatrixFromPops(Populations * pops);

#endif //ZZ1GAME_FIELD_H
