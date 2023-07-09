#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "src/field.h"
#include "src/gfx.h"
#include "src/nrand.h"

#include<unistd.h>

#ifdef __SWITCH__
#include <switch.h>
#endif

// switch buttons
#define JOY_A     0
#define JOY_B     1
#define JOY_X     2
#define JOY_Y     3
#define JOY_PLUS  10
#define JOY_MINUS 11
#define JOY_LEFT  12
#define JOY_UP    13
#define JOY_RIGHT 14
#define JOY_DOWN  15

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
	
    if (pkmn) SDL_DestroyTexture(pkmn);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
	if (n > 1) TTF_Quit();
    if (n > 0) IMG_Quit();
    SDL_Quit();
	exit(n < 3);
}

int main(int argc, char ** argv) {
	(void) argc;
	(void) argv;
	
	#ifdef __SWITCH__
    romfsInit();
    chdir("romfs:/");
	#endif

    int seed = (argc > 1) ? atoi(argv[1]) : time(NULL);
    snrand(seed);
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
	font = TTF_OpenFont("assets/font.ttf", FontSize);
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
	
    pkmn = IMG_LoadTexture(renderer,"assets/pkmn.png");
    if (!pkmn) {
        SDL_Log("Echec du chargement de l'image dans la texture: %s\n", SDL_GetError()); // texture de la SDL a échoué
        sdl_exit(2);
    }

    Brain b[3] = {{.eval = 0}, {.eval = 0}, {.eval = 0}};

    Populations pops;

    vitesse = 10;

    int gameState = 1;      // Etat du jeu : 0 = Jeu, 1 = Menu

	SDL_bool run = SDL_TRUE, // Booléen pour dire que le programme doit continuer
	paused = SDL_FALSE,      // Booléen pour dire que le programme est en pause
	event_utile,             // Booléen pour savoir si on a trouvé un event traité
    end = SDL_FALSE;         // Booléen pour savoir si une équipe a gagné
	SDL_Event event;         // Evènement à traiter
	
	//printf("Load best...\n");
	//get_all_best_brain(Bests);
	/*for (int i = 0; i < 3; i++) {
		for (int u = 0; u < 3; u++) {
			Bests[i][u] =
		}
	}*/
	
	#ifdef __SWITCH__
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_JoystickOpen(0);
	#endif

	while (run) {
		event_utile = SDL_FALSE;
		
		while (!event_utile && SDL_PollEvent(&event)) {
			
			switch (event.type) {
				case SDL_QUIT: {
					EXIT:
					run=SDL_FALSE;
					event_utile=SDL_TRUE;
					break;
				}
				case SDL_JOYBUTTONDOWN:
					switch(event.jbutton.button){
						case JOY_PLUS:
							goto EXIT;
						case JOY_MINUS:
							goto PAUSE;
						case JOY_UP:
						case JOY_B:
							goto UP;
						case JOY_DOWN:
						case JOY_A:
							goto DOWN;
						case JOY_LEFT:
							goto LEFT;
						case JOY_RIGHT:
							goto RIGHT;
						default: break;
					}
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym){
						case SDLK_p:
						case SDLK_SPACE:
							PAUSE:
							paused = paused ? SDL_FALSE : SDL_TRUE;
							event_utile = SDL_TRUE;
							break;
							case SDLK_ESCAPE:
						case SDLK_q:
							run = SDL_FALSE;
							event_utile = SDL_TRUE;
							break;
						case SDLK_LEFT:
							LEFT:
                            if(!gameState) {
                                if (vitesse == 10) vitesse = 20;
                                else if (vitesse == 5) vitesse = 10;
                                else if (vitesse == 3) vitesse = 5;
                                else if (vitesse == 1) vitesse = 3;
                            } else{
                                if(menu_color[menu_y] > 0) menu_color[menu_y] --;
                            }
							break;
						case SDLK_RIGHT:
							RIGHT:
                            if(!gameState) {
                                if (vitesse == 20) vitesse = 10;
                                else if (vitesse == 10) vitesse = 5;
                                else if (vitesse == 5) vitesse = 3;
                                else if (vitesse == 3) vitesse = 1;
                            } else{
                                if(menu_color[menu_y] < 2) menu_color[menu_y] ++;
                            }
							break;
						case SDLK_DOWN:
                        case SDLK_RETURN :
							DOWN:
                            if(gameState == 1){
                                menu_y ++;
                            }
                            break;
						case SDLK_UP:
                        case SDLK_BACKSPACE :
							UP:
                            if(gameState == 1){
                                if (menu_y) menu_y --;
                            }
                            break;
						default: break;
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
        } else{

            drawMenu(renderer);
            SDL_RenderPresent(renderer);

            if(menu_y >= 3){
                menu_y = 0;
                gameState = 0;

                for(int i = 0; i < 3; i++){
                    if(menu_color[i] == 0) rand_brain(b + i);
                    else if(menu_color[i] == 1) load_brain(b + i, -2, i + 1, Glouton);
                    else if(menu_color[i] == 2) load_brain(b + i, -2, i + 1, AlgoG);

                    printf("Cerveau %s :\n", (!i) ? "rouge" : ((i == 1) ? "vert" : "bleu") );
                    printBrain(b+i);
                    printf("------------------------------\n");
                }
                Brain *brain[3] = {&b[0], &b[1], &b[2]};

                create_pops(&pops, brain, 0);

                update_status(&pops);
                predict_move(&pops);
				SDL_Delay(5);
            }
        }
		
		animSprite++;
		if (animSprite >= FramePerSprite * 3) animSprite = 0;
		//SDL_Delay(10);
	}

    printf("Nombre itérations : %d\n", pops.iteration);
	#ifdef __SWITCH__
    romfsExit();
	#endif
	sdl_exit(3);
	return 0;
}
