#ifndef ZZ1GAME_GFX_H
#define ZZ1GAME_GFX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "field.h"

#define FontSize 50

extern int WIDTH, HEIGHT;
extern int originX, originY;

extern int sizeCaseGrid;

extern int iterAnim, vitesse;

extern int Padding;
extern TTF_Font * font;

void setup(SDL_DisplayMode dmode);

void draw(SDL_Renderer * renderer, Populations * pops, SDL_bool end);

#endif //ZZ1GAME_GFX_H