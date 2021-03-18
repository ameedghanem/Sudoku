/*
 * auxiliary.c
 *
 *  the source file used with auxiliary.h
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "auxiliary.h"



/*
 * accepts 2 2d arrays and boardSize. assumes both are BSxBS
 * copies old into new.
 */
void mem2dcpy(int** new, int** old, int boardSize)
{
	int i;
	for(i=0; i<boardSize; i++){
		memcpy(new[i], old[i], boardSize*sizeof(int));
	}
}

/* allocates an empty board the size n*n for presumably n=rows*columns
 * freeing this space is up to the caller for obvious reasons.
 */
int** allocateEmptyBoard2d(int boardSize)
{
	int i;
	int** emptyBoard;
	emptyBoard = calloc(boardSize, sizeof(int*));
	for (i = 0; i < boardSize; i++) {
		emptyBoard[i] = calloc(boardSize, sizeof(int));
	}
	return emptyBoard;
}


/*
 * changes all non-zero values in a board to be "fixed" => negative
 */
void fixAllValues(int** board, int boardSize)
{
	int i,j;
	for (i = 0; i < boardSize; i++) {
		for (j = 0; j < boardSize; j++) {
			if (board[i][j] > 0)
				board[i][j] = -board[i][j];
		}
	}
}


/*
 * frees the insides of a 2d board boardSizexboardSize. note it doesn't free the outside array
 */
void free2dArray(int** board, int boardSize)
{
	int i;
	for(i=0; i<boardSize; i++){
		free(board[i]);
	}
}

/*
 * frees the insides of a state
 */
void freeState(gameState* state)
{
	freeStateExecptBottomNode(state);
	free(state->bottomNode);
}

/*
 * is separated from freeState for usage when switching between puzzles
 */
void freeStateExecptBottomNode(gameState* state)
{
	int boardSize = state->rows*state->columns;
	if(state->board!=NULL){
		free2dArray(state->board, boardSize);
		free(state->board);
	}
	state->currentNode = state->bottomNode;
	freeDLLBeyond(state->bottomNode);
}

/*
 * sets all boards values to 0
 */
void zeroBoard(int** board, int boardSize)
{
	int i, j;
	for(i=0; i<boardSize;i++){
		for(j=0;j<boardSize;j++){
			board[i][j]=0;
		}
	}
}

/*
 * finds the block that the cell [i,j] is in.
 */
int* findBlock(int rows, int columns, int i, int j)
{
	int* ret = calloc(2, sizeof(int));
	ret[0] = i;
	ret[1] = j;
	if(i%rows != 0){
		ret[0] -= i%rows;
	}
	if(j%columns != 0){
		ret[1] -= j%columns;
	}
	return ret;
}

/*
 * returns 1 <-> value val is valid
 */
int isValidValue(int** board, int rows, int columns, int row, int column, int val)
{
	int i=0, j=0, m=0, n=0, *a, boardSize = rows*columns, tmp=0;
	int answer = 1;
	if(val==0){ /*it is always valid to empty a non-fixed cell*/
		return answer;
	}
	a = findBlock(rows, columns, row, column);
	n = a[0];
	m = a[1];
	tmp = board[row][column]; /*to prevent something not being a valid value where it already is there*/
	board[row][column] = 0;
	for(i=0; i < boardSize; i++){
		if(board[row][i] == val || board[row][i] == val*(-1)){ /*checking against -v for fixed values*/
			answer = 0;
			break;
		}
	}

	if (answer) { /*no point looping again if answer=0 from previously*/
		for(i=0; i < boardSize; i++){
			if(board[i][column] == val || board[i][column] == val*(-1)){
				answer = 0;
				break;
			}
		}
	}

	if (answer) {
		for(i=n; i < n+rows; i++){
			for(j=m; j < m+columns; j++){
				if(board[i][j] == val || board[i][j] == val*(-1)){
					free(a);
					answer = 0;
					board[row][column] = tmp;
					return answer;
				}
			}
		}
	}
	board[row][column] = tmp;
	free(a);
	return answer;
}


/*
 * returns 1 <-> Board contains conflicting values.
 */
int isErroneousBoard(gameState state)
{
	int i, j, boardSize;
	boardSize = state.rows*state.columns;
	for(i=0; i<boardSize; i++){
		for(j=0; j<boardSize; j++){
			if(!isValidValue(state.board, state.rows, state.columns, i, j, state.board[i][j])){
				return 1;
			}
		}
	}
	return 0;
}

/*
 * return 1 <-> there are no empty cells
 */
int isFilledBoard(gameState state)
{
	int i, j, boardSize = state.rows*state.columns;
	for(i=0; i<boardSize; i++){
		for(j=0; j<boardSize; j++){
			if(state.board[i][j] == 0){
				return 0;
			}
		}
	}
	return 1;
}

/*
 * returns 1 <-> every cell has 0
 */
int isEmptyBoard(gameState state)
{
	int i, j, boardSize = state.rows*state.columns;
	for(i=0; i<boardSize; i++){
		for(j=0; j<boardSize; j++){
			if(state.board[i][j] != 0){
				return 0;
			}
		}
	}
	return 1;
}
