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
	None = 0,
	Win = 1,
	Lose = 2,
	Kamikaze = 3
} EndState;

typedef struct {
	Rule status;
	int x, y, nx, ny, alive;
	Dir action;
} Individual;

/**
 * @struct State
 * @brief a state for a population
 * @var State::victims
 * the number of opponents killed
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

typedef union {
	struct {
		Population r, g, b;
	};
	Population pops[3];
	int iteration;
} Populations;

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

void mutation (Brain brain);

void hybridization (Brain parent1, Brain parent2,Brain child);

/**
 * @brief save the given brain to ./brains/<level>.<species>
 * @param [in] brain the brain to save
 * @param [in] level the level (= nb of evolution of the brain)
 * @param [in] species the species associate to the brain
 * @return 1 if saved, 0 otherwise
 * */
int save_brain(Brain brain, int level, Species species);

/**
 * @brief load the given brain from ./brains/<level>.<species>
 * @param [in] brain the brain to load
 * @param [in] level the level (= nb of evolution of the brain)
 * @param [in] species the species associate to the brain
 * @return 1 if loaded, 0 otherwise
 * */
int load_brain(Brain brain, int level, Species species);

/**
 * @brief get the last level of the saved brains of the specified species
 * @param [in] species the species
 * @return the last level or -1
 * */
int get_last_brain(Species species);

#endif //ZZ1GAME_POPULATION_H
