#ifndef ZZ1GAME_GFX_H
#define ZZ1GAME_GFX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "field.h"

#define FontSize 40

extern int WIDTH, HEIGHT;
extern int originX, originY;

extern int sizeCaseGrid;

extern int iterAnim, vitesse;

extern int Padding;
extern TTF_Font * font;

extern int menu_color[3], menu_y;
extern SDL_Texture * pkmn;
extern SDL_Rect Sprites[3][4][3];
extern int animSprite;

extern int Bests[3][3];

void setup(SDL_DisplayMode dmode);

void draw(SDL_Renderer * renderer, Populations * pops, SDL_bool end);

void drawMenu(SDL_Renderer * renderer);

#endif //ZZ1GAME_GFX_H
