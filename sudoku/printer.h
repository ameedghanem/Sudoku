/*
 * printer.h
 *
 *  Created on: 15 בספט 2018
 *      Author: owner
 */

#ifndef PRINTER_H_
#define PRINTER_H_

#include "game.h"

#endif /* PRINTER_H_ */

/*
 * this enumerator represents the 'stock' messages - the messages that do not require specific input.
 * all such messages are printed by messagePrinter.
 */
typedef enum mid_game_message {
	fixed_cell,
	invalid_value,
	solved_successfully,
	validation_unsolvable_board,
	validation_solvable_board,
	exiting,
	is_invalid_command, /*added 'is' to not conflict with the COMMAND enumerator*/
	no_moves_to_undo,
	no_moves_to_redo,
	board_reset,
	sudoku,
	enter_command,
	file_inaccessible,
	file_cant_be_opened,
	binary_value,
	solved_wrong,
	contains_wrong_values,
	board_not_empty,
	generator_failed,
	board_validation_failed, /*for save. different from validation unsolvable board*/
	file_immutable,
	cell_contains_a_value,
	unsolvable_board,
	good_board,
	multiple_solutions

} MESSAGE;


/*functions that deal with printing the board*/
void printSeparateRow(int rows, int columns);
void printSeparateBlock();
void printFixedValue(int val);
void printValue(int val);
void printErroneousValue(int val);
void printBoard(int** board, int rows, int columns, int markErrors);
void printBoardFromState(gameState state);
void printRow(int** solvedBoard, int row, int blockSize, int boardSize, int markErrors);


/*functions that deal with printing messages*/
void messagePrinter(MESSAGE m);
void undoRedoMessage(gameState* state, int isUndo);

