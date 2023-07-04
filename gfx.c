#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_image.h>

#include "gfx.h"

int WIDTH, HEIGHT;
int originX, originY;

int sizeCaseGrid;

void setup(SDL_DisplayMode dmode){
    WIDTH = dmode.w;
    HEIGHT = dmode.h;
    originX = WIDTH / 2 - HEIGHT / 2;
    originY = 0;

    DISTMAXFIELD = sqrt(2) * SIZEMAP;

    sizeCaseGrid = (HEIGHT - (SIZEMAP - 1) * 2) / SIZEMAP;
}

void drawGrid(SDL_Renderer * renderer){
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_Rect terrain = {originX, originY, HEIGHT, HEIGHT};
    SDL_RenderFillRect(renderer, &terrain);

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    for(int i = 0; i < SIZEMAP - 1; i++){
        SDL_Rect gridV = {originX + sizeCaseGrid + i * (sizeCaseGrid + 2), originY, 2, HEIGHT};
        SDL_RenderFillRect(renderer, &gridV);
        SDL_Rect gridH = {originX, originY + sizeCaseGrid + i * (sizeCaseGrid + 2), HEIGHT, 2};
        SDL_RenderFillRect(renderer, &gridH);
    }
}

void drawPops(SDL_Renderer * renderer, Populations * pops){
    for(int k = 0; k < 3; k++) {
        if(pops->pops[k].species == RED) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        else if(pops->pops[k].species == GREEN) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        else SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for (int i = 0; i < IndividualPerPopulation; i++) {
            int x = pops->pops[k].individuals[i].x;
            int y = pops->pops[k].individuals[i].y;

            SDL_Rect indiv = {originX + x * (sizeCaseGrid + 2), originY + y * (sizeCaseGrid + 2), sizeCaseGrid, sizeCaseGrid};
            SDL_RenderFillRect(renderer, &indiv);
        }
    }
}

void draw(SDL_Renderer * renderer, Populations * pops){
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    drawGrid(renderer);

    drawPops(renderer, pops);
}