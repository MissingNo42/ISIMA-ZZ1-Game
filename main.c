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
    font = TTF_OpenFont("./assets/font.ttf", FontSize);
    if (!font) {
        SDL_Log("Couldn't load font - %s\n", SDL_GetError());
        sdl_exit(2);
    }

    int kami = 1;
    int seed = 0;
    while(kami) {
        srand(seed);
        printf("seed = %d\n", seed);


        SDL_DisplayMode dmode;
        SDL_GetCurrentDisplayMode(0, &dmode);

        setup(dmode);

        //printf("Screen size: %dx%d\n", WIDTH, HEIGHT);  // taille écran

        /*
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
         */

        /*
        texture = IMG_LoadTexture(renderer,"./assets/balls.png");
        map = IMG_LoadTexture(renderer,"./assets/map.png");
        if (!texture || !map) {
            SDL_Log("Echec du chargement de l'image dans la texture: %s\n", SDL_GetError()); // texture de la SDL a échoué
            sdl_exit(2);
        }*/

        Brain b[3] = {{.eval = 0},
                      {.eval = 0},
                      {.eval = 0}};

        rand_brain(b + 0);
        rand_brain(b + 1);
        rand_brain(b + 2);

        Population popRed = {.individuals = {
                {.x = 0, .y = SIZEMAP - 1, .alive = 1},
                {.x = 2, .y = SIZEMAP - 1, .alive = 1},
                {.x = 4, .y = SIZEMAP - 1, .alive = 1},
                {.x = 1, .y = SIZEMAP - 2, .alive = 1},
                {.x = 6, .y = SIZEMAP - 1, .alive = 1},
        }, b + 0,
           RED, .state = {.end_state = None, .alives = IndividualPerPopulation, .targets = IndividualPerPopulation}};

        Population popGreen = {.individuals = {
                {.x = SIZEMAP - 1, .y = SIZEMAP - 1, .alive = 1},
                {.x = SIZEMAP - 3, .y = SIZEMAP - 1, .alive = 1},
                {.x = SIZEMAP - 5, .y = SIZEMAP - 1, .alive = 1},
                {.x = SIZEMAP - 2, .y = SIZEMAP - 2, .alive = 1},
                {.x = SIZEMAP - 4, .y = SIZEMAP - 2, .alive = 1},
        }, b + 1,
           GREEN, .state = {.end_state = None, .alives = IndividualPerPopulation, .targets = IndividualPerPopulation}};

        Population popBlue = {.individuals = {
                {.x = 10, .y = 0, .alive = 1},
                {.x = 10, .y = 1, .alive = 1},
                {.x = 12, .y = 1, .alive = 1},
                {.x = 9, .y = 2, .alive = 1},
                {.x = 11, .y = 2, .alive = 1},
        }, b + 2,
           BLUE, .state = {.end_state = None, .alives = IndividualPerPopulation, .targets = IndividualPerPopulation}};

        Populations pops = {.pops = {popRed, popGreen, popBlue}, .iteration = 0};

        vitesse = 1;

        SDL_bool run = SDL_TRUE, // Booléen pour dire que le programme doit continuer
        paused = SDL_FALSE,      // Booléen pour dire que le programme est en pause
        event_utile = SDL_FALSE, // Booléen pour savoir si on a trouvé un event traité
        end = SDL_FALSE;         // Booléen pour savoir si une équipe a gagné
        SDL_Event event;         // Evènement à traiter

        predict_move(&pops);

        while (run) {
            event_utile = SDL_FALSE;

            while (!event_utile && SDL_PollEvent(&event)) {

                switch (event.type) {
                    case SDL_QUIT: {
                        run = SDL_FALSE;
                        event_utile = SDL_TRUE;
                        break;
                    }
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym) {
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
                            case SDLK_UP:
                                break;
                            case SDLK_DOWN:
                                break;
                            default:
                                break;
                        }
                        break;
                    case SDL_KEYUP:
                        switch (event.key.keysym.sym) {
                            case SDLK_LEFT:
                                break;
                            case SDLK_RIGHT:
                                break;
                            case SDLK_UP:
                                break;
                            case SDLK_DOWN:
                                break;
                            default:
                                break;
                        }
                        break;
                }
            }

            if (!paused && !end) {

                eat_move(&pops);
                execute_move(&pops);

                predict_move(&pops);

                if (is_terminated(&pops)) end = SDL_TRUE;


                //draw(renderer, &pops, end);
                //SDL_RenderPresent(renderer);
                //SDL_FlushEvents(SDL_KEYDOWN, SDL_KEYUP - 1);
            }
            if (end && pops.pops[0].state.end_state == 1) printf("RED GAGNE\n");
            else if (end && pops.pops[1].state.end_state == 1) printf("GREEN GAGNE\n");
            else if (end && pops.pops[2].state.end_state == 1) printf("BLUE GAGNE\n");

            //SDL_Delay(10);
            if (end && pops.pops[0].state.end_state == 2) {
                printf("RED KAMIKAZE : Seed = %d\n", seed);
                kami = 0;
            } else if (end && pops.pops[1].state.end_state == 2) {
                printf("GREEN KAMIKAZE : Seed = %d\n", seed);
                kami = 0;
            } else if (end && pops.pops[2].state.end_state == 2) {
                printf("BLUE KAMIKAZE : Seed = %d\n", seed);
                kami = 0;
            } else if (end) {
                printf("Non, seed suivante...\n");
                run = SDL_FALSE;
            }
        }
        //if (renderer) SDL_DestroyRenderer(renderer);
        //if (window) SDL_DestroyWindow(window);
        seed++;
    }

    sdl_exit(3);
    //printf("Nombre itérations : %d\n", pops.iteration);
	return 0;
}
#endif
