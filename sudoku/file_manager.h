/*
 * file_manager.h
 *
 * this header and source hold all functions that deal with files.
 */

#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

#include "game_logic.h"

#endif /* FILE_MANAGER_H_ */


void saveToFile(gameState state, FILE* fp);
void loadFromFile(gameState* state, FILE* fp);
