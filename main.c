#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "field.h"
#include "gfx.h"

#define FontSize 50

SDL_Window * window;
SDL_Renderer * renderer;

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
int main(int argc, char ** argv) {
	(void) argc;
	(void) argv;

    int seed = time(NULL);
    //int seed = 38528;
    srand(seed);
    printf("seed = %d\n", seed);
	
	/* Initialisation de la SDL + gestion de l'échec possible */
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
	font = TTF_OpenFont("./assets/font.ttf", FontSize);
	if (!font) {
		SDL_Log("Couldn't load font - %s\n", SDL_GetError());
		sdl_exit(2);
	}

	
	SDL_DisplayMode dmode;
	SDL_GetCurrentDisplayMode(0, &dmode);

	setup(dmode);

	printf("Screen size: %dx%d\n", WIDTH, HEIGHT);  // taille écran
	
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

    /*
    //Balanced
    Rule r0 = {.raw = {0,2,-1,-1,-1,-1, 0, 5}};
    Rule r1 = {.raw = {1,2,-1,-1,-1,-1, 1, 5}};
    Rule r2 = {.raw = {2,2,-1,-1,-1,-1, 2, 5}};
    Rule r3 = {.raw = {3,2,-1,-1,-1,-1, 3, 5}};
    Rule r4 = {.raw = {-1,-1,0,2,-1,-1, 2, 5}};
    Rule r5 = {.raw = {-1,-1,1,2,-1,-1, 3, 5}};
    Rule r6 = {.raw = {-1,-1,2,2,-1,-1, 0, 5}};
    Rule r7 = {.raw = {-1,-1,3,2,-1,-1, 1, 5}};
    Rule r8 = {.raw = {-1,-1,-1,-1,0,-1, 0, 2}};
    Rule r9 = {.raw = {-1,-1,-1,-1,1,-1, 1, 2}};
    Rule r10 = {.raw = {-1,-1,-1,-1,2,-1, 2, 2}};
    Rule r11 = {.raw = {-1,-1,-1,-1,3,-1, 3, 2}};
    Rule r12 = {.raw = {0,-1,-1,-1,-1,-1, 0, 0}};
    Rule r13 = {.raw = {1,-1,-1,-1,-1,-1, 1, 0}};
    Rule r14 = {.raw = {2,-1,-1,-1,-1,-1, 2, 0}};
    Rule r15 = {.raw = {3,-1,-1,-1,-1,-1, 3, 0}};


    //Defense
    Rule r0 = {.raw = {0,-1,-1,-1,-1,-1, 0, 5}};
    Rule r1 = {.raw = {1,-1,-1,-1,-1,-1, 1, 5}};
    Rule r2 = {.raw = {2,-1,-1,-1,-1,-1, 2, 5}};
    Rule r3 = {.raw = {3,-1,-1,-1,-1,-1, 3, 5}};
    Rule r4 = {.raw = {-1,-1,-1,-1,-1,-1, -1, 0}};
    Rule r5 = {.raw = {-1,-1,-1,-1,-1,-1, -1, 0}};
    Rule r6 = {.raw = {-1,-1,-1,-1,-1,-1, -1, 0}};
    Rule r7 = {.raw = {-1,-1,-1,-1,-1,-1, -1, 0}};
    Rule r8 = {.raw = {-1,-1,-1,-1,-1,-1, -1, 0}};
    Rule r9 = {.raw = {-1,-1,-1,-1,-1,-1, -1, 0}};
    Rule r10 = {.raw = {-1,-1,-1,-1,-1,-1, -1, 0}};
    Rule r11 = {.raw = {-1,-1,-1,-1,-1,-1, -1, 0}};
    Rule r12 = {.raw = {-1,-1,-1,-1,-1,-1, -1, 0}};
    Rule r13 = {.raw = {-1,-1,-1,-1,-1,-1, -1, 0}};
    Rule r14 = {.raw = {-1,-1,-1,-1,-1,-1, -1, 0}};
    Rule r15 = {.raw = {-1,-1,-1,-1,-1,-1, -1, 0}};

    Brain preDef;
    preDef.rules[0] = r0;
    preDef.rules[1] = r1;
    preDef.rules[2] = r2;
    preDef.rules[3] = r3;
    preDef.rules[4] = r7;
    preDef.rules[5] = r6;
    preDef.rules[6] = r5;
    preDef.rules[7] = r4;
    preDef.rules[8] = r11;
    preDef.rules[9] = r10;
    preDef.rules[10] = r9;
    preDef.rules[11] = r8;
    preDef.rules[12] = r15;
    preDef.rules[13] = r14;
    preDef.rules[14] = r13;
    preDef.rules[15] = r12;


    save_brain(&preDef, 1, RED);
     */

    Brain b[3] = {{.eval = 0}, {.eval = 0}, {.eval = 0}};

    //rand_brain(b + 0);
    //rand_brain(b + 1);
    //rand_brain(b + 2);

    load_brain(b + 0, get_last_brain(RED), RED);
    load_brain(b + 1, get_last_brain(RED), RED);
    load_brain(b + 2, get_last_brain(RED), RED);

    printBrain(b+0);

    Brain *brain[3] = {&b[0], &b[1], &b[2]};

    Populations pops;
    create_pops(&pops, brain, 0);


    vitesse = 10;
	
	SDL_bool run = SDL_TRUE, // Booléen pour dire que le programme doit continuer
	paused = SDL_FALSE,      // Booléen pour dire que le programme est en pause
	event_utile = SDL_FALSE, // Booléen pour savoir si on a trouvé un event traité
    end = SDL_FALSE;         // Booléen pour savoir si une équipe a gagné
	SDL_Event event;         // Evènement à traiter

    update_status(&pops);
    predict_move(&pops);

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
                            if (vitesse == 10) vitesse = 20;
                            else if (vitesse == 5) vitesse = 10;
                            else if (vitesse == 3) vitesse = 5;
                            else if (vitesse == 1) vitesse = 3;
							break;
						case SDLK_RIGHT:
                            if (vitesse == 20) vitesse = 10;
                            else if (vitesse == 10) vitesse = 5;
                            else if (vitesse == 5) vitesse = 3;
                            else if (vitesse == 3) vitesse = 1;
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
		
		if (!paused && !end) {

            iterAnim ++;
            if(iterAnim > vitesse){
                iterAnim = 0;

                eat_move(&pops);
                execute_move(&pops);

                update_status(&pops);
                predict_move(&pops);

                if(is_terminated(&pops)) end = SDL_TRUE;
            }

            draw(renderer, &pops, end);
			SDL_RenderPresent(renderer);
			//SDL_FlushEvents(SDL_KEYDOWN, SDL_KEYUP - 1);
		}

		//SDL_Delay(10);
	}

    printf("Nombre itérations : %d\n", pops.iteration);
	sdl_exit(3);
	return 0;
}
#endif
