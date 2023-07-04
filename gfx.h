#ifndef ZZ1GAME_GFX_H
#define ZZ1GAME_GFX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "field.h"

extern int WIDTH, HEIGHT;
extern int originX, originY;

extern int sizeCaseGrid;

void setup(SDL_DisplayMode dmode);

void draw(SDL_Renderer * renderer, Populations * pops);

#endif //ZZ1GAME_GFX_H