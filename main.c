#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

#define FontSize 50

SDL_Window * window;
SDL_Renderer * renderer;
TTF_Font * font;

SDL_Window * new_window(char * title, int x, int y, int w, int h, Uint32 flags) {
	SDL_Window * wd = SDL_CreateWindow(title, x, y, w, h, flags);
	if (!wd) {
		SDL_Log("Error : SDL window '%s' creation - %s\n", title, SDL_GetError());
		SDL_Quit(); // On referme la SDL
		exit(EXIT_FAILURE);
	}
	return wd;
}

/**
 * @brief clean closing of the SDL and freeing of the associated memory
 * @param [in] n if we have initialize somes externs SDL libraries
 * */
void sdl_exit(char n){
	if (font) TTF_CloseFont(font);
	
    //if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
	if (n > 1) TTF_Quit();
    if (n > 0) IMG_Quit();
    SDL_Quit();
	exit(n < 3);
}


#ifndef TESTING
#ifndef TRAINING
int main(int argc, char ** argv) {
	(void) argc;
	(void) argv;
	
	/* Initialisation de la SDL  + gestion de l'échec possible */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("Error : SDL initialisation - %s\n", SDL_GetError()); // l'initialisation de la SDL a échoué
		exit(EXIT_FAILURE);
	}
	
	if (!IMG_Init(IMG_INIT_PNG)) {
        SDL_Log("Error : SDL-Img initialisation - %s\n", SDL_GetError());
        sdl_exit(0);
    }
	
	if (TTF_Init() < 0) {
		SDL_Log("Couldn't initialize SDL TTF - %s\n", SDL_GetError());
		sdl_exit(1);
	}
	/*
	font = TTF_OpenFont("./assets/font.ttf", FontSize);
	if (!font) {
		SDL_Log("Couldn't load font - %s\n", SDL_GetError());
		sdl_exit(2);
	}*/

	
	SDL_DisplayMode dmode;
	SDL_GetCurrentDisplayMode(0, &dmode);
	
	
	int WIDTH = dmode.w, HEIGHT = dmode.h; // taille écran
	printf("Screen size: %dx%d\n", WIDTH, HEIGHT);
	
    window = new_window("Title", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN);
    if (!window) {
        SDL_Log("Error : SDL create window - %s\n", SDL_GetError()); // fenetre de la SDL a échoué
        sdl_exit(2);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("Error : SDL create renderer - %s\n", SDL_GetError()); // renderer de la SDL a échoué
        sdl_exit(2);
    }
	
	/*
    texture = IMG_LoadTexture(renderer,"./assets/balls.png");
    map = IMG_LoadTexture(renderer,"./assets/map.png");
    if (!texture || !map) {
        SDL_Log("Echec du chargement de l'image dans la texture: %s\n", SDL_GetError()); // texture de la SDL a échoué
        sdl_exit(2);
    }*/
	
	SDL_bool run = SDL_TRUE, // Booléen pour dire que le programme doit continuer
	paused = SDL_FALSE,      // Booléen pour dire que le programme est en pause
	event_utile = SDL_FALSE; // Booléen pour savoir si on a trouvé un event traité
	SDL_Event event;         // Evènement à traiter
	
	while (run) {
		event_utile = SDL_FALSE;
		
		while (!event_utile && SDL_PollEvent(&event)) {
			
			switch (event.type) {
				case SDL_QUIT: {
					run=SDL_FALSE;
					event_utile=SDL_TRUE;
					break;
				}
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym){
						case SDLK_p:
						case SDLK_SPACE:
							paused=paused?SDL_FALSE:SDL_TRUE;
							event_utile=SDL_TRUE;
							break;
							case SDLK_ESCAPE:
						case SDLK_q:
								run=SDL_FALSE;
								event_utile=SDL_TRUE;
								break;
						case SDLK_LEFT:
							break;
						case SDLK_RIGHT:
							break;
						case SDLK_UP:break;
						case SDLK_DOWN:break;
						default: break;
					}
					break;
				case SDL_KEYUP:
					switch(event.key.keysym.sym){
						case SDLK_LEFT: break;
						case SDLK_RIGHT: break;
						case SDLK_UP: break;
						case SDLK_DOWN:break;
						default:break;
					}
					break;
			}
		}
		
		if (!paused) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer);
			//SDL_FlushEvents(SDL_KEYDOWN, SDL_KEYUP - 1);
		}
		
		SDL_Delay(20);
	}

	sdl_exit(3);
	return 0;
}
#endif
#endif
