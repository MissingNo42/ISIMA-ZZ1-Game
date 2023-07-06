#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "gfx.h"

int WIDTH, HEIGHT;
int originX, originY;

int sizeCaseGrid;

int iterAnim, vitesse;

int Padding;
TTF_Font * font = NULL;

int menu_color[3], menu_y;

void setup(SDL_DisplayMode dmode){
    WIDTH = dmode.w;
    HEIGHT = dmode.h;
    originX = WIDTH / 2 - HEIGHT / 2;
    originY = 0;

    DISTMAXFIELD = sqrt(2) * SIZEMAP;

    sizeCaseGrid = (HEIGHT - (SIZEMAP - 1) * 2) / SIZEMAP;

    iterAnim = 0;
    Padding = HEIGHT / 20;

    menu_color[0] = 0;
    menu_color[1] = 0;
    menu_color[2] = 0;
    menu_y = 0;
}

void drawGrid(SDL_Renderer * renderer){
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_Rect terrain = {originX, originY, HEIGHT, HEIGHT};
    SDL_RenderFillRect(renderer, &terrain);

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    for (int i = 0; i < SIZEMAP - 1; i++) {
        SDL_Rect gridV = {originX + sizeCaseGrid + i * (sizeCaseGrid + 2), originY, 2, HEIGHT};
        SDL_RenderFillRect(renderer, &gridV);
        SDL_Rect gridH = {originX, originY + sizeCaseGrid + i * (sizeCaseGrid + 2), HEIGHT, 2};
        SDL_RenderFillRect(renderer, &gridH);
    }
}

void drawPops(SDL_Renderer * renderer, Populations * pops){
	SDL_SetRenderDrawColor(renderer, 180+ iterAnim, 180+ iterAnim, 180+ iterAnim, 255);
	for (int x = 0; x < SIZEMAP; x++){
		for (int y = 0; y < SIZEMAP; y++){
			if (pops->field.map[x][y] == -1){
				SDL_Rect indiv = {originX + x * (sizeCaseGrid + 2), originY + y * (sizeCaseGrid + 2), sizeCaseGrid,
                                  sizeCaseGrid};
                SDL_RenderFillRect(renderer, &indiv);
			}
		}
	}
    for(int k = 0; k < 3; k++) {
        if(pops->pops[k].species == RED) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        else if(pops->pops[k].species == GREEN) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        else SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for (int i = 0; i < IndividualPerPopulation; i++) {
            if(pops->pops[k].individuals[i].alive) {
                int x = pops->pops[k].individuals[i].x;
                int y = pops->pops[k].individuals[i].y;

                SDL_Rect indiv = {originX + x * (sizeCaseGrid + 2), originY + y * (sizeCaseGrid + 2), sizeCaseGrid,
                                  sizeCaseGrid};
                SDL_RenderFillRect(renderer, &indiv);
            }
        }
    }
}

void drawMouv(SDL_Renderer * renderer, Populations * pops){
	SDL_SetRenderDrawColor(renderer, 180+ iterAnim, 180+ iterAnim, 180+ iterAnim, 255);
	for (int x = 0; x < SIZEMAP; x++){
		for (int y = 0; y < SIZEMAP; y++){
			if (pops->field.map[x][y] == -1){
				SDL_Rect indiv = {originX + x * (sizeCaseGrid + 2), originY + y * (sizeCaseGrid + 2), sizeCaseGrid,
                                  sizeCaseGrid};
                SDL_RenderFillRect(renderer, &indiv);
			}
		}
	}
	
	
	SDL_SetRenderDrawColor(renderer, 50- iterAnim, 50- iterAnim, 50- iterAnim, 255);
	for (int x = 0; x < SIZEMAP; x++){
		for (int y = 0; y < SIZEMAP; y++){
			if (pops->field.cache[pops->pops[0].individuals[0].x][pops->pops[0].individuals[0].y][x][y] == 1){
				SDL_Rect indiv = {originX + x * (sizeCaseGrid + 2), originY + y * (sizeCaseGrid + 2), sizeCaseGrid,
                                  sizeCaseGrid};
                SDL_RenderFillRect(renderer, &indiv);
			}
		}
	}
	
	
    for(int k = 0; k < 3; k++) {
        if(pops->pops[k].species == RED) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        else if(pops->pops[k].species == GREEN) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        else SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for (int i = 0; i < IndividualPerPopulation; i++) {
            if(pops->pops[k].individuals[i].alive) {
                int xDiff = (pops->pops[k].individuals[i].nx - pops->pops[k].individuals[i].x) * (sizeCaseGrid + 2) /
                            vitesse;
                int yDiff = (pops->pops[k].individuals[i].ny - pops->pops[k].individuals[i].y) * (sizeCaseGrid + 2) /
                            vitesse;

                SDL_Rect indiv = {originX + pops->pops[k].individuals[i].x * (sizeCaseGrid + 2) + iterAnim * xDiff,
                                  originY + pops->pops[k].individuals[i].y * (sizeCaseGrid + 2) + iterAnim * yDiff,
                                  sizeCaseGrid, sizeCaseGrid};
                SDL_RenderFillRect(renderer, &indiv);
            }
        }
    }
}

//CC romain
void drawText(SDL_Renderer * renderer, SDL_Rect * pos, SDL_Color * color, const char * const str, ...) {
    char txt[1024];
    va_list l;
    va_start(l, str);
    vsnprintf(txt, 1024, str, l);
    va_end(l);

    SDL_Surface * text_surface = TTF_RenderText_Blended(font, txt,
                                                        color ? *color : (SDL_Color) {255, 255, 255, 255});
    if (text_surface) {
        SDL_Texture * text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_FreeSurface(text_surface);
        if (text_texture) {
            SDL_QueryTexture(text_texture, NULL, NULL, &pos->w, &pos->h);
            SDL_RenderCopy(renderer, text_texture, NULL, pos);
            SDL_DestroyTexture(text_texture);
        }
    }
}

void drawInfos(SDL_Renderer * renderer, Populations * pops, SDL_bool end){
    SDL_Color color = {255, 255, 255, 255};

    SDL_Rect dst = {Padding, Padding * 2, 0, 0};
    drawText(renderer, &dst, &color, "Iteration");

    dst.y += FontSize + Padding;
    drawText(renderer, &dst, &color, "%d", pops->iteration);

    dst.y += FontSize + Padding * 4;
    drawText(renderer, &dst, &color, "Vitesse :");

    dst.y += FontSize + Padding;
    drawText(renderer, &dst, &color, "%s %s %s %s %s",(vitesse <= 20) ? ">" : "",
                                                    (vitesse <= 10) ? ">" : "",
                                                    (vitesse <= 5) ? ">" : "",
                                                    (vitesse <= 3) ? ">" : "",
                                                    (vitesse <= 1) ? ">" : ""
             );

    if(end){
        dst.x = originX + HEIGHT + Padding - 10;
        dst.y = Padding * 2;
        drawText(renderer, &dst, &color, "Status :");
        for(int i = 0; i < 3; i++) {
            dst.y += FontSize + Padding;
            drawText(renderer, &dst, &color, "%s : %s",
                     (pops->pops[i].species == 1) ? "RED" : ((pops->pops[i].species == 2) ? "GREEN" : "BLUE"),
                     (pops->pops[i].state.end_state == -1) ? "Lose" : ((pops->pops[i].state.end_state == 0) ? "Neutral"
                                                                                                            : ((pops->pops[i].state.end_state ==
                                                                                                                1)
                                                                                                               ? "Win"
                                                                                                               : "Kamikaze")));
        }
    }
}

void draw(SDL_Renderer * renderer, Populations * pops, SDL_bool end){
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    drawGrid(renderer);

    if(iterAnim == 0) drawPops(renderer, pops);
    else drawMouv(renderer, pops);

    drawInfos(renderer, pops, end);
}

void drawInfosMenu(SDL_Renderer * renderer){
    SDL_Color color = {255, 255, 255, 255};

    SDL_Rect dst = {Padding * 2, Padding * 2, 0, 0};
    drawText(renderer, &dst, &color, "Selection des Populations");

    char * tabText[] = {"Aleatoire", "Glouton Mut 1", "Glouton Mut 2 ", "Genetique"};
    dst.y += FontSize + Padding;

    for(int i = 0; i < 3; i++){
        dst.y += (int)(FontSize + Padding * 1.5);

        color = (SDL_Color) {255, 255, 255, 255};
        if(menu_y == i){
            dst.x = Padding * 3;
            drawText(renderer, &dst, &color, ">");
        }
        dst.x = Padding * 4;

        for(int j = 0; j < 4; j++){
            if(j == 1) dst.x += Padding * 6;
            else if(j > 1) dst.x += Padding * 8;
            color = (SDL_Color) {255, 255, 255, 255};
            if(menu_color[i] == j) color = (SDL_Color) {255 * (i == 0), 255 * (i == 1), 255 * (i == 2), 255};
            drawText(renderer, &dst, &color, "%s", tabText[j]);
        }
    }
}

void drawMenu(SDL_Renderer * renderer){
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderClear(renderer);

    drawInfosMenu(renderer);
}
