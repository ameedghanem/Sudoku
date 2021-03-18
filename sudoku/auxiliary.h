/*
 * auxiliary.h
 *
 * this header and source hold auxiliary functions that are seperated into categories below
 * info about individual functions can be found before them in the source
 */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include "game_logic.h"

#endif /* MAIN_AUX_H_ */




/*boardwide manipulations and allocating functions*/
void mem2dcpy(int** new, int** old, int boardSize);
int** allocateEmptyBoard2d(int boardSize);
void zeroBoard(int** board, int boardSize);
void fixAllValues(int** board, int boardSize);


/*freeing functions*/
void free2dArray(int** board, int boardSize);
void freeState(gameState* state);
void freeStateExecptBottomNode(gameState* state);

/*single-cell functions*/
int* findBlock(int rows, int columns, int i, int j);
int isValidValue(int** board, int rows, int columns, int row, int column, int val);

/*board condition checkers*/
int isCompletedBoard(gameState state);
int isFilledBoard(gameState state);
int isErroneousBoard(gameState state);
int isEmptyBoard(gameState state);

