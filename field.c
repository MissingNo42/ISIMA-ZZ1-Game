#include <stdio.h>
#include <stdlib.h>



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

int main(){
    int size = 20;
    int ** field = createField(size);
    printField(field, size);
    return 0;
}