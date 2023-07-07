//
// Created by Romain on 03/07/2023.
//

#ifndef ZZ1GAME_POPULATION_H
#define ZZ1GAME_POPULATION_H

#include "rules.h"

#define IndividualPerPopulation 5

typedef enum {
	RED = 1,
	GREEN = 2,
	BLUE = 3,
} Species;

typedef enum {
	Glouton = 1,
	AlgoG = 2,
} TypeAI;

typedef enum {
	Grouped,
	Dispatch,
} Locator;

typedef enum {
	None = 0,
	Win = 1,
	Lose = -1,
	Kamikaze = 2
} EndState;

typedef struct {
	Rule status;
	int x, y, nx, ny, alive;
	Dir action;
} Individual;

/**
 * @struct State
 * @brief a state for a population
 * @var State::targets
 * the number of remaining opponents
 * @var State::alives
 * the number of dead
 * @var State::end_state
 * the state of the population at the end
 * */
typedef struct {
	int targets, alives;
	EndState end_state;
} State;

typedef struct {
	Individual individuals[IndividualPerPopulation];
	Brain * brain;
	Species species;
	State state;
} Population;

typedef struct {
	union {
		struct {
			Population r, g, b;
		};
		Population pops[3];
	};
	int iteration;
} Populations;


typedef struct {
	Brain *brain[BrainPool2];
	Brain prey, predator;
	int level; // generation
	Species species;
} Brains;


/**
 * @brief prepare the action: check environment collision and pre-perform the move in id->nx/ny
 * @param [in,out] id the individual to prepare the action
 * */
void prepare_move(Individual * id);

/**
 * @brief choose an action for all individuals and prepare its execution
 * @param [in, out] pops the populations set
 * */
void predict_move(Populations * pops);

void eat_move(Populations * populations);

void execute_move (Populations * populations );

void move(Populations * pops);

void eval(Populations * pops, int ind);

/**
 * @brief check if the game is terminated and compute result if needed
 * @param [in] pops the population set
 * @param [in, out] states the current state
 * @return 0 if the game continue, 1 if terminated and return the result
 * */
int is_terminated(Populations * pops);

/**
 * @brief execute a complete simulation
 * @param [in, out] pops the population to simulate
 * @return the number of iterations
 * */
int simulate(Populations * pops);

void mutation (Brain * brain);

int mutation_one(Brains * brains, int L);

void mutation_all (Brains * brains, int* list_ind, Species species);

void change_path_random(int * list, int size);

void mutation_two_do (Brains * brains, int* list_ind);

void select_best(Brains * brains, int nb);

void hybridization(Brain * parent1, Brain * parent2, Brain * child);

void hybridization2(Brain * parent1, Brain * parent2, Brain * child);

void hybridization3(Brain * parent1, Brain * parent2, Brain * parent3, Brain * child);

/**
 * @brief save the given brain to ./brains/<level>.<species>
 * @param [in] brain the brain to save
 * @param [in] level the level (= nb of evolution of the brain)
 * @param [in] species the species associate to the brain
 * @param [in] type the type of the AI
 * @return 1 if saved, 0 otherwise
 * */
int save_brain(Brain * brain, int level, Species species, TypeAI type);

/**
 * @brief load the given brain from ./brains/<algo>.<level>.<species>
 * @param [in] brain the brain to load
 * @param [in] level the level (= nb of evolution of the brain) or -1 (last) or -2 (best)
 * @param [in] species the species associate to the brain
 * @param [in] type the type of the AI
 * @return the loaded level if loaded, 0 otherwise
 * */
int load_brain(Brain * brain, int level, Species species, TypeAI type);

/**
 * @brief get the last level of the saved brains of the specified species
 * @param [in] species the species
 * @param [in] type the type of the AI
 * @return the last level or -1
 * */
int get_last_brain(Species species, TypeAI type);

/**
 * @brief get the best level of the saved brains of the specified species
 * @param [in] species the species
 * @param [in] type the type of the AI
 * @return the best level or -1
 * */
int get_best_brain(Species species, TypeAI type);

/**
 * @brief get the best level of the saved brains of the specified species
 * */
void get_all_best_brain(int Best[3][2]);

/**
 * @brief initialize individuals positions to be dispatched and alone in a 3x3 square
 * @param [out] pops the populations
 * @return the same populations
 * */
Populations * init_dispatched_pops(Populations * pops);

/**
 * @brief initialize individuals positions to be grouped
 * @param [out] pops the populations
 * @param [in] offset the offset to select the triangle corner
 * @return the same populations
 * */
Populations * init_grouped_pops(Populations * pops, int offset);

Populations * create_pops(Populations * pops, Brain *brain[3], int decal);

#endif //ZZ1GAME_POPULATION_H
