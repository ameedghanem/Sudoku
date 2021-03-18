/*
 * game_logic.h
 *
 *  this header and source hold the gameState struct, which holds all the data the app needs
 *   as well as functions that manipulate the data of gameState to fulfill the users commands
 *   info about individual functions can be found before them in the source
 */

#ifndef GAME_LOGIC_H_
#define GAME_LOGIC_H_

#include "DLL.h"

typedef enum gameMode {
	init_mode,
	edit_mode,
	solve_mode
} GAMEMODE;


typedef struct{
	int rows;
	int columns;
	int** board;
	DLLNode* bottomNode;
	DLLNode* currentNode;
	GAMEMODE mode;
	int markErrors;
}gameState;


#endif /* GAME_LOGIC_H_ */


int getNumOfSolutions(gameState state);
void autofillBoard(gameState* state);
int generatePuzzle(gameState* state, int x, int y);
void editNewBoard(gameState* state, int rows, int columns);


/*managing the undo/redo DLL*/
void incrementUndo(gameState* state, int row, int column, int newV);
void undoMove(gameState* state);
void redoMove(gameState* state);

