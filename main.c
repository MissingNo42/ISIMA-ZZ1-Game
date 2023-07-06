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
#ifndef TRAINING
int main(int argc, char ** argv) {
	(void) argc;
	(void) argv;

    int seed = time(NULL);
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

    window = new_window("Simulation", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN);
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

    Brain b[3] = {{.eval = 0}, {.eval = 0}, {.eval = 0}};

    Populations pops;

    vitesse = 10;

    int gameState = 1;      // Etat du jeu : 0 = Jeu, 1 = Menu

	SDL_bool run = SDL_TRUE, // Booléen pour dire que le programme doit continuer
	paused = SDL_FALSE,      // Booléen pour dire que le programme est en pause
	event_utile = SDL_FALSE, // Booléen pour savoir si on a trouvé un event traité
    end = SDL_FALSE;         // Booléen pour savoir si une équipe a gagné
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
							paused = paused ? SDL_FALSE : SDL_TRUE;
							event_utile = SDL_TRUE;
							break;
							case SDLK_ESCAPE:
						case SDLK_q:
								run = SDL_FALSE;
								event_utile = SDL_TRUE;
								break;
						case SDLK_LEFT:
                            if(!gameState) {
                                if (vitesse == 10) vitesse = 20;
                                else if (vitesse == 5) vitesse = 10;
                                else if (vitesse == 3) vitesse = 5;
                                else if (vitesse == 1) vitesse = 3;
                            } else if(gameState == 1){
                                if(menu_color[menu_y] > 0) menu_color[menu_y] --;
                            }
							break;
						case SDLK_RIGHT:
                            if(!gameState) {
                                if (vitesse == 20) vitesse = 10;
                                else if (vitesse == 10) vitesse = 5;
                                else if (vitesse == 5) vitesse = 3;
                                else if (vitesse == 3) vitesse = 1;
                            } else if(gameState == 1){
                                if(menu_color[menu_y] < 3) menu_color[menu_y] ++;
                            }
							break;
						case SDLK_UP:break;
						case SDLK_DOWN:break;
                        case SDLK_RETURN :
                            if(gameState == 1){
                                menu_y ++;
                            }
                            break;
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

        if(!gameState) {
            if (!paused && !end) {

                iterAnim++;
                if (iterAnim > vitesse) {
                    iterAnim = 0;

                    eat_move(&pops);
                    execute_move(&pops);

                    update_status(&pops);
                    predict_move(&pops);

                    if (is_terminated(&pops)) end = SDL_TRUE;
                }

                draw(renderer, &pops, end);
                SDL_RenderPresent(renderer);
                //SDL_FlushEvents(SDL_KEYDOWN, SDL_KEYUP - 1);
            }
        } else if(gameState == 1){

            drawMenu(renderer);
            SDL_RenderPresent(renderer);

            if(menu_y >= 3){
                menu_y = 0;
                gameState = 0;

                for(int i = 0; i < 3; i++){
                    if(menu_color[i] == 0) rand_brain(b + i);
                    else if(menu_color[i] == 1) load_brain(b + i, get_last_brain(i + 1, 1), i + 1);
                    else if(menu_color[i] == 2) load_brain(b + i, get_last_brain(i + 1, 2), i + 1);
                    else if(menu_color[i] == 3) load_brain(b + i, get_last_brain(i + 1, 3), i + 1);

                    printf("Cerveau %s :\n", (!i) ? "rouge" : ((i == 1) ? "vert" : "bleu") );
                    printBrain(b+i);
                    printf("------------------------------\n");
                }
                Brain *brain[3] = {&b[0], &b[1], &b[2]};

                create_pops(&pops, brain, 0);

                update_status(&pops);
                predict_move(&pops);
            }
        }

		//SDL_Delay(10);
	}

    printf("Nombre itérations : %d\n", pops.iteration);
	sdl_exit(3);
	return 0;
}
#endif
#endif
