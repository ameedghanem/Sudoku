/*
 * file_manager.c
 *
 *  the source file used with file_manager.h
 */

#include<stdio.h>
#include<stdlib.h>
#include "file_manager.h"
#include "auxiliary.h"


/*
 * saves the board in state to file fp in the format specified in the pdf.
 * pre: fp!=NULL
 */
void saveToFile(gameState state, FILE* fp)
{
	int i, j, v, flag, boardSize = state.rows*state.columns, testingMode=0;

	if (fp == NULL && testingMode){
		printf("something terribly wrong has happened");
		return;
	}

	flag = fprintf(fp, "%d %d\n", state.rows, state.columns);
	if (flag<0  && testingMode){
		printf("failed to write the first row");
	}

	for(i=0; i<boardSize; i++){
		for(j=0; j<boardSize; j++){
			v = state.board[i][j];
			if (v<0 || ((state.mode == edit_mode) && v>0)){
				if (v<0){
					v=-v;
				}
				flag = fprintf(fp, "%d.", v);

			}else{
				flag = fprintf(fp, "%d", v);
			}

			if (flag<0 && testingMode){
				printf("failed to write the cell [%d,%d]", i, j);
			}

			if (j!=boardSize-1){
				flag = fputc(' ', fp);
				if (flag<0 && testingMode){
					printf("failed to write the space after [%d,%d]", i, j);
				}
			}
		}
		if (i!=boardSize-1){
			flag = fputc('\n', fp);
			if (flag<0 && testingMode){
				printf("failed to write the newline after row %d", i);
			}
		}
	}
}

/*
 * pre: the file, if the pointer isn't NULL, is formatted correctly (it can, as required in the pdf, have too-many/wrong-types-of whitespaces)
 * , in a mode that allows reading, and at the files start.
 *
 * loads the board from the file, frees the old board, and resets the DLL
 */
void loadFromFile(gameState* state, FILE* fp)
{
	int i, j, v, flag, boardSize, testingMode=0;
	int** newBoard;
	if (fp == NULL && testingMode){
		printf("something terribly wrong has happened");
		return;
	}

	freeStateExecptBottomNode(state);

	while((flag = fscanf(fp, "%d", &v))!= EOF){
		if (flag==1){
			state->rows = v;
			break;
		}
	}
	if (flag == EOF && testingMode){
		printf("failed to read the row number");
	}

	while((flag = fscanf(fp, "%d", &v))!= EOF){
		if (flag==1){
			state->columns = v;
			break;
		}
	}
	if (flag == EOF && testingMode){
		printf("failed to read the column number");
	}

	boardSize = state->rows*state->columns;
	newBoard = allocateEmptyBoard2d(boardSize);

	for(i=0; i<boardSize; i++){
		for(j=0; j<boardSize; j++){
			flag = fscanf(fp, "%d", &v);
			if (flag == 0){
				j--;
				continue; /*this is a newline eater*/
			}

			if (flag == EOF && testingMode){
				printf("error during reading the cell [%d,%d]",i,j); /*for testing purposes. remove before submission*/
				return;
			}

			newBoard[i][j] = v;
			v = fgetc(fp);
			/*there must be at least 1 space between cells in the file so this is either a delimiter of some sort (space/tab/newline..) or this is a '.'.
			 * if it's a delimiter, it just gets eaten and we don't mind */
			if (v == '.'){
				newBoard[i][j] = -newBoard[i][j]; /*fixed value*/
			}
		}
	}

	state->board = newBoard;
}
