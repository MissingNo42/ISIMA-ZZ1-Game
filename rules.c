//
// Created by Romain on 03/07/2023.
//
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include "rules.h"
#include "population.h"


/**
 * @brief check if a status match the given rule
 * @param [in] status the status
 * @param [in] rule the rule
 * @return 1 if the status matches the rule, 0 otherwise
 * */
int match(Rule * status, Rule * rule) {
	int r = 1;
	if (status->ally.dir != JOKER && rule->ally.dir != JOKER) r &= status->ally.dir == rule->ally.dir;
	if (r && status->ally.dist != ANY && rule->ally.dist != ANY) r &= status->ally.dist == rule->ally.dist;

	if (r && status->prey.dir != JOKER && rule->prey.dir != JOKER) r &= status->prey.dir == rule->prey.dir;
	if (r && status->prey.dist != ANY && rule->prey.dist != ANY) r &= status->prey.dist == rule->prey.dist;

	if (r && status->predator.dir != JOKER && rule->predator.dir != JOKER) r &= status->predator.dir == rule->predator.dir;
	if (r && status->predator.dist != ANY && rule->predator.dist != ANY) r &= status->predator.dist == rule->predator.dist;
	return r;
}

/**
 * @brief select the rule to execute
 * @param [in] status the current status of the individual
 * @param [in] brain the brain associated to the individual
 * @return the rule to execute or -1 if none
 * */
int choice_rule(Rule * status, Brain brain){
	float probaN = 0, probaD = 0;
	int matched[P];
	for (int i = 0; i < P; i++) {
		matched[i] = match(status, brain + i);
		if (matched[i]) probaD += powf(brain[i].priority + 1, ProbaExp);
	}
	int select = rand();

	int last = -1;
	for (int i = 0; i < P; i++) {
		if (matched[i]) {
			last = i;
			probaN += powf(brain[i].priority + 1, ProbaExp);
			if (select * probaD <= probaN * (float)RAND_MAX) return i;
		}
	}
	return last;
}

void rand_rule (Rule * rule){
    rule->raw[0] = rand() % 5 - 1;
    rule->raw[1] = rand() % 4 - 1;
    rule->raw[2] = rand() % 5 - 1;
    rule->raw[3] = rand() % 4 - 1;
    rule->raw[4] = rand() % 5 - 1;
    rule->raw[5] = rand() % 4 - 1;
    rule->raw[6] = rand() % 4;
    rule->raw[7] = rand() % (MAX_PRIORITY + 1);
}



/**
 * @brief save the given brain to ./brains/<level>.<species>
 * @param [in] brain the brain to save
 * @param [in] level the level (= nb of evolution of the brain)
 * @param [in] species the species associate to the brain
 * @return 1 if saved, 0 otherwise
 * */
int save_brain(Brain brain, int level, Species species) {
	char str[42];
	int r = 1;
	
	mkdir("brains", 0744);
	snprintf(str, 32, "brains/%06d.%s", level, (species == RED) ? "red": ((species == BLUE) ? "blue": "green"));
	
	FILE * f = fopen(str, "wb");
	if (f) {
		r &= fwrite(brain, sizeof(Brain), 1, f) == sizeof(Brain);
		fclose(f);
	} else r = 0;
	return r;
}


/**
 * @brief load the given brain from ./brains/<level>.<species>
 * @param [in] brain the brain to load
 * @param [in] level the level (= nb of evolution of the brain)
 * @param [in] species the species associate to the brain
 * @return 1 if loaded, 0 otherwise
 * */
int load_brain(Brain brain, int level, Species species) {
	char str[42];
	int r = 1;
	
	snprintf(str, 32, "brains/%06d.%s", level, (species == RED) ? "red": ((species == BLUE) ? "blue": "green"));
	
	FILE * f = fopen(str, "rb");
	if (f) {
		r &= fread(brain, sizeof(Brain), 1, f) == sizeof(Brain);
		fclose(f);
	} else r = 0;
	return r;
}

/**
 * @brief get the last level of the saved brains of the specified species
 * @param [in] species the species
 * @return the last level or -1
 * */
int get_last_brain(Species species) {
	char extw[10];
	int lvl, lvl_max = -1;
	
	DIR *d = opendir("brains");
	struct dirent *dir;
	
	if (d) {
		snprintf(extw, 10, ".%s", (species == RED) ? "red": ((species == BLUE) ? "blue": "green"));
		while ((dir = readdir(d)) != NULL) {
			char * ext;
			lvl = (int)strtol(dir->d_name, &ext, 10);
			if (lvl < lvl_max && strcmp(ext, extw) == 0) lvl_max = lvl; //sscanf(dir->d_name, "%d.%*c", &lvl);
		}
		closedir(d);
	}
	return lvl_max;
}
