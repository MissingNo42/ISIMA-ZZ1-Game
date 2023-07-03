#include <stdio.h>
#include <stdlib.h>

#include "rules.h"

void freeField(int ** field, int size){
    for(int i = 0; i < size; i++){
        free(field[i]);
    }
    free(field);
}

int ** createField(int size){
    int ** field = malloc(sizeof(int *) * size);
    if(field) {
        for (int i = 0; i < size; i++) {
            field[i] = calloc(sizeof(int), size);
            if(!field[i]) freeField(field, i - 1);
        }
    }
    return field;
}

void printField(int ** field, int size){
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++) printf("----");
        printf("-\n|");
        for(int j = 0; j < size; j++){
            if(field[i][j]) printf(" %d |", field[i][j]);
            else printf("   |");
        }
        printf("\n");
    }
    for(int i = 0; i < size; i++) printf("----");
    printf("-\n");
}

void nearestPrey(int ** field, Individual indiv, Population popPrey){
    int color = field[indiv.i][indiv.j];
    indiv.prey.dir;
    indiv.prey.dist;
    for(int k = 0; k < size; k++){
        for(int i = -k; i < k; i++){
            for(int j = -k; j < k; j++){
                if((i != -k || i != k) && (j == -k || j == k)) {
                    if (x + i > 0 && x + i < size && y + i > 0 && y + i < size &&
                        field[x + i][y + j] == (color + 1) % 3) {
                        if (i + j > 0) {
                            if (i - j > 0) prey.dir = N;
                            else prey.dir = W;
                        } else if (i + j < 0) {
                            if (i - j >= 0) prey.dir = E;
                            else prey.dir = S;
                        } else {
                            if (i - j > 0) prey.dir = N;
                            else if (i - j < 0) prey.dir = S;
                        }
                    }
                }
            }
        }
    }
}

int main(){
    int size = 20;
    int ** field = createField(size);
    printField(field, size);
    return 0;
}