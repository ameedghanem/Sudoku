/*
 * game.c
 *
*  the source file used with game.h
 */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include "game.h"
#include "ILPsolver.h"
#include "auxiliary.h"
#include "printer.h"
#include "file_manager.h"


/*
 * implements the command in block in state by directing it to the corresponding MC function.
 * if that MC function returns 1, it also prints the board
 */
void setCommands(gameState* state, commandBlock block)
{
	int answer=0;
	switch(block.command){
		case invalid_command:
			messagePrinter(is_invalid_command);
			break;

		case solve:
			answer = MCsolve(state, block.path);
			break;

		case edit:
			answer = MCedit(state, block.path);
			break;

		case mark_errors:
			answer = MCmark_errors(state, block.integerComponent[0]);
			free(block.integerComponent);
			break;

		case print_board:
			answer = MCprint_board(state);
			break;

		case set:
			answer = MCset(state, block.integerComponent[0]-1, block.integerComponent[1]-1, block.integerComponent[2]);
			free(block.integerComponent);
			break;

		case validate:
			answer = MCvalidate(state);
			break;

		case generate:
			answer = MCgenerate(state, block.integerComponent[0], block.integerComponent[1]);
			free(block.integerComponent);
			break;

		case undo:
			answer = MCundo(state);
			break;

		case redo:
			answer = MCredo(state);
			break;

		case save:
			answer = MCsave(state, block.path);
			break;

		case hint:
			answer = MChint(state, block.integerComponent[0]-1, block.integerComponent[1]-1);
			free(block.integerComponent);
			break;

		case num_solutions:
			answer = MCnum_solutions(state);
			break;

		case autofill:
			answer = MCautofill(state);
			break;

		case reset:
			answer = MCreset(state);
			break;

		case exit_command:
			answer = MCexit();
			break;
	}
	if(answer){
		printBoardFromState(*state);
	}
}

/*information regarding all MC functions is in the header*/

int MCsolve(gameState* state, char* path)
{
	FILE* fp;
	fp = fopen(path, "r");
	if (fp == NULL){
		messagePrinter(file_inaccessible);
		return 0;
	}

	loadFromFile(state, fp);
	state->mode = solve_mode;

	fclose(fp);
	return 1;
}

int MCedit(gameState* state, char* path)
{
	FILE* fp;
	if (path == NULL ) { /*starting editing an empty 9x9 board, as per specifications*/

		editNewBoard(state, 3, 3);/*intentionally 3x3, as specified in the forums*/
		state->mode = edit_mode;
		return 1;
	}
	fp = fopen(path, "r");
	if (fp == NULL ) {
		messagePrinter(file_cant_be_opened);
		return 0;
	}
	loadFromFile(state, fp);
	state->mode = edit_mode;

	fclose(fp);
	return 1;
}

int MCmark_errors(gameState* state, int toMark)
{
	if (state->mode != solve_mode) {
		messagePrinter(is_invalid_command);
	} else if (toMark != 0	&& toMark != 1) {
		messagePrinter(binary_value);
	} else {
		state->markErrors = toMark;
	}
	return 0;
}

int MCprint_board(gameState* state) {
	if (state->mode == init_mode) {
		messagePrinter(is_invalid_command);
		return 0;
	}
	return 1;
}

int MCset(gameState* state, int i, int j, int v)
{
	int boardSize = state->rows*state->columns;

	/*if (v == 1337) {
		//cheatcode to get to a board which isn't 3x3 without using an existing file, since generate requires an existing empty board and edit [no path] results in a 3x3
		editNewBoard(state, i + 1, j + 1);
		state->mode = solve_mode;
		return 1;
	}*/

	if (state->mode == init_mode) {
		messagePrinter(is_invalid_command);
		return 0;

	} else if (i > boardSize - 1 || i < 0 || j > boardSize - 1 || j < 0) {
		printf("Error: value not in range 1-%d\n", boardSize);
		return 0;

	} else if (v > boardSize || v < 0) {
		printf("Error: value not in range 0-%d\n", boardSize);
		return 0;

	} else if (state->mode != edit_mode && state->board[j][i] < 0) { /*in edit mode we are allowed to change a fixed cells value*/
		messagePrinter(fixed_cell);
		return 0;
	}

	incrementUndo(state, j, i, v); /*on the forum it was answered that if set didn't change anything we may choose whether to add an entry to the DLL. we do.*/
	state->board[j][i] = v;
	printBoardFromState(*state); /*done here rather than outside because we need to do it before checking for completion*/

	if (state->mode == solve_mode && isFilledBoard(*state)) {
		if (!isErroneousBoard(*state)) {
			state->mode = init_mode;
			messagePrinter(solved_successfully);
		} else {
			messagePrinter(solved_wrong);
		}
	}
	return 0;
}

int MCvalidate(gameState* state)
{
	if (state->mode == init_mode) {
		messagePrinter(is_invalid_command);
		return 0;
	} else if (isErroneousBoard(*state)) {
		messagePrinter(contains_wrong_values);
		return 0;
	}

	if (isSolvableBoard(state) == 1) {
		messagePrinter(validation_solvable_board);
		return 0;
	} else {
		messagePrinter(validation_unsolvable_board);
		return 0;
	}
	return 0;
}

int MCgenerate(gameState* state, int i, int j)
{
	int boardSize = state->rows*state->columns;

	if (state->mode != edit_mode) {
		messagePrinter(is_invalid_command);
		return 0;

	} else if (i > boardSize * boardSize || i < 0 || j > boardSize * boardSize || j < 0) {/*since the board must be empty for this command to execute, E=boardSize*boardSize*/
		printf("Error: value not in range 0-%d\n", boardSize * boardSize);
		return 0;

	} else if (!isEmptyBoard(*state)) {
		messagePrinter(board_not_empty);
		return 0;

	} else if (!generatePuzzle(state, i, j)) {
		printf("Error: puzzle generator failed\n");
		return 0;
	}
	return 1;
}

int MCundo(gameState* state)
{
	if (state->mode == init_mode) {
		messagePrinter(is_invalid_command);
		return 0;
	}
	if (state->currentNode != state->bottomNode) {
		undoMove(state);
		return 0;
	} else {
		messagePrinter(no_moves_to_undo);
		return 0;
	}
}

int MCredo(gameState* state)
{
	if (state->mode == init_mode) {
		messagePrinter(is_invalid_command);
		return 0;
	}
	if (state->currentNode->nextN) {
		redoMove(state);
		return 0;

	} else {
		messagePrinter(no_moves_to_redo);
		return 0;
	}
}

int MCsave(gameState* state,char* path)
{
	FILE* fp;
	if (state->mode == init_mode) {
		messagePrinter(is_invalid_command);
		return 0;
	}

	if (state->mode == edit_mode) {
		if (isErroneousBoard(*state)) {
			messagePrinter(contains_wrong_values);
			return 0;
		}
		if (!isSolvableBoard(state)) {
			messagePrinter(board_validation_failed);
			return 0;
		}
	}

	fp = fopen(path, "w");
	if (fp == NULL ) {
		messagePrinter(file_immutable);
		return 0;
	}
	saveToFile(*state, fp);
	printf("Saved to: %s\n", path);
	fclose(fp);
	return 0;
}

int MChint(gameState* state, int i, int j)
{
	int result, boardSize = state->rows*state->columns;
	int **backupBoard;

	if (state->mode != solve_mode) {
		messagePrinter(is_invalid_command);
		return 0;

	} else if (i >= boardSize || j >= boardSize || i < 0 || j < 0) {
		printf("Error: value not in range 1-%d\n", boardSize);
		return 0;

	} else if (isErroneousBoard(*state)) {
		messagePrinter(contains_wrong_values);
		return 0;

	} else if (state->board[j][i] < 0) {
		messagePrinter(fixed_cell);
		return 0;

	} else if (state->board[j][i] > 0) {
		messagePrinter(cell_contains_a_value);
		return 0;
	}else{
		backupBoard = allocateEmptyBoard2d(boardSize);
		mem2dcpy(backupBoard, state->board, boardSize);

		result = ILPsolver(state);
		if(result!=1){
			messagePrinter(unsolvable_board);
			mem2dcpy(state->board, backupBoard, boardSize);
			free2dArray(backupBoard, boardSize);
			free(backupBoard);
			return 0;
		}else{
			printf("Hint: set cell to %d\n", state->board[j][i]);
			free2dArray(state->board, boardSize);
			free(state->board);
			state->board = backupBoard; /*the original board is solved by the ILPsolver, so we change to the backup to retain the original board state*/
			return 0;
		}
	}
}

int MCnum_solutions(gameState* state)
{
	int tmp;
	if (state->mode == init_mode) {
		messagePrinter(is_invalid_command);
		return 0;

	} else if (isErroneousBoard(*state)) {
		messagePrinter(contains_wrong_values);
		return 0;
	}

	tmp = getNumOfSolutions(*state);
	printf("Number of solutions: %d\n", tmp);

	if (tmp == 1) {
		messagePrinter(good_board);
	} else if (tmp > 1) {
		messagePrinter(multiple_solutions);
	}
	return 0;
	/*confirmed in project forum that if there are 0 solutions nothing additional should be printed*/
}

int MCautofill(gameState* state)
{
	if (state->mode != solve_mode) {
		messagePrinter(is_invalid_command);
		return 0;
	} else if (isErroneousBoard(*state)) {
		messagePrinter(contains_wrong_values);
		return 0;
	}

	autofillBoard(state);

	if (isFilledBoard(*state)) {
		if (!isErroneousBoard(*state)) {
			state->mode = init_mode;
			messagePrinter(solved_successfully);
		} else {
			messagePrinter(solved_wrong);
		}
	}
	return 1;
}

int MCreset(gameState* state)
{
	if (state->mode == init_mode) {
		messagePrinter(is_invalid_command);
		return 0;
	}
	while (state->currentNode != state->bottomNode) {
		undoMove(state);
	}
	freeDLLBeyond(state->currentNode);
	messagePrinter(board_reset);
	return 1;
}

int MCexit()
{
	messagePrinter(exiting);
	return 0;
}


