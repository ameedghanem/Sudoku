/*
 * printer.c
 *
 *  the source file used with printer.h
 */

#include<stdio.h>
#include "printer.h"
#include "auxiliary.h"

/*
 * prints the value as is
 */
void printValue(int val)
{
	printf("%2d ", val);
}

/*
 * prints the value with . denoting a fixed value
 */
void printFixedValue(int val)
{
	printf("%2d.", val);
}

/*
 * print the value with * denoting an erroneous value
 */
void printErroneousValue(int val)
{
	printf("%2d*", val);
}

/*
 * prints out a series of dashes "-" depending on boardSize and blockSize
 * in case of 9*9 board it will be 34 dashes
 */
void printSeparateRow(int rows, int columns)
{
	int i;
	for(i=0; i<(rows*columns*4+columns+1); i++){
		printf("-");
	}
	printf("\n");
}

/*
 * prints separator block. duh.
 */
void printSeparateBlock()
{
	printf("|");
}

/*
 * prints out a single row from the board
 * for a value v, a fixed value is -v, and a valid value
 * is +v. an empty cell has a value of 0.
 */
void printRow(int** board, int row, int rows, int columns, int markErrors)
{
	int i, j;
	printSeparateBlock();
	for(i=0; i<rows; i++){
		for(j=0; j<columns; j++){
			printf(" ");
			if((i*columns + j)>=rows*columns){
				printf("indexOutOfRange");
			}
			if (board[row][i*columns + j]>0){
				if (markErrors){ /*only a non-fixed value can be marked as erroneous*/
					if (isValidValue(board, rows, columns, row, i*columns + j, board[row][i*columns + j])){
						printValue(board[row][i*columns + j]);

					}else{
						printErroneousValue(board[row][i*columns + j]);
					}

				}else{
					printValue(board[row][i*columns + j]);
				}

			} else if (board[row][i*columns + j]<0){
				printFixedValue(board[row][i*columns + j]*(-1));

			}else{
				printf("   "); /*3 spaces rather than 2 because it also includes the space for not having , or * */
			}
		}
		printSeparateBlock();
	}
	printf("\n");
}

/*
 * separated from printBoard to be able to print a board used within a function without a corresponding state
 */
void printBoardFromState(gameState state){
	printBoard(state.board, state.rows, state.columns, (state.markErrors||state.mode == edit_mode)); /*we always mark errors in edit mode*/
}

/*
 * prints the board. duh.
 */
void printBoard(int** board, int rows, int columns, int markErrors)
{
	int  k, rowNum = 0, num = columns*(rows+1)+1;
	for (k = 1; k <= num; k++) {
		if (k % (rows + 1) == 1) {
			printSeparateRow(columns, rows);
		} else {
			printRow(board, rowNum, rows, columns, markErrors);
			rowNum++;
		}
	}
}

/*
 * prints the message received
 */
void messagePrinter(MESSAGE m)
{
	switch (m){
		case fixed_cell:
			printf("Error: cell is fixed\n");
			break;
		case invalid_value:
			printf("Error: value is invalid\n");
			break;
		case solved_successfully:
			printf("Puzzle solved successfully\n");
			break;
		case validation_unsolvable_board:
			printf("Validation failed: board is unsolvable\n");
			break;
		case validation_solvable_board:
			printf("Validation passed: board is solvable\n");
			break;
		case exiting:
			printf("Exiting...\n");
			break;
		case is_invalid_command:
			printf("ERROR: invalid command\n");
			break;
		case no_moves_to_undo:
			printf("Error: no moves to undo\n");
			break;
		case no_moves_to_redo:
			printf("Error: no moves to redo\n");
			break;
		case board_reset:
			printf("Board reset\n");
			break;
		case sudoku:
			printf("Sudoku\n------\n");
			break;
		case enter_command:
			printf("Enter your command:\n");
			break;
		case file_inaccessible:
			printf("Error: File doesn't exist or cannot be opened\n"); /*capital F as in pdf*/
			break;
		case file_cant_be_opened:
			printf("Error: File cannot be opened\n"); /*capital F as in pdf*/
			break;
		case binary_value:
			printf("Error: the value should be 0 or 1\n");
			break;
		case solved_wrong:
			printf("Puzzle solution erroneous\n");
			break;
		case contains_wrong_values:
			printf("Error: board contains erroneous values\n");
			break;
		case board_not_empty:
			printf("Error: board is not empty\n");
			break;
		case generator_failed:
			printf("Error: puzzle generator failed\n");
			break;
		case board_validation_failed:
			printf("Error: board validation failed\n");
			break;
		case file_immutable:
			printf("Error: File cannot be created or modified\n"); /*capital F as in pdf*/
			break;
		case cell_contains_a_value:
			printf("Error: cell already contains a value\n");
			break;
		case unsolvable_board:
			printf("Error: board is unsolvable\n");
			break;
		case good_board:
			printf("This is a good board!\n");
			break;
		case multiple_solutions:
			printf("The puzzle has more than 1 solution, try to edit it further\n");
			break;
	}
}

/*
 * prints the message that follows a single undo/redo action. not implemented through messagePrinter because it uses additional values
 */
void undoRedoMessage(gameState* state, int isUndo)
{
	int oldV,newV;
	if (isUndo){
		oldV = state->currentNode->postV;
		newV = state->currentNode->prevV;
	}else{
		newV = state->currentNode->postV;
		oldV = state->currentNode->prevV;
	}

	(isUndo) ? printf("Undo") : printf("Redo");
	printf(" %d,%d: from ", state->currentNode->column + 1, state->currentNode->row + 1);
	(oldV) ? printf("%d", oldV) : printf("_");
	printf(" to ");
	(newV) ? printf("%d\n", newV) : printf("_\n");
}

