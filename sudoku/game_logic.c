/*
 * game_logic.c
 *
 *  the source file used with game_logic.h
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "game_logic.h"
#include "auxiliary.h"
#include "ILPsolver.h"
#include "printer.h"

/*
 * generates a puzzle, returns 1 for success and 0 if it failed to generate
 * corrupts the current (presumably empty) board
 *
 * x = amount of cells to be filled before ILP solver
 * y = amount of cells to keep after ILP solver
 */
 int generatePuzzle(gameState* state, int x, int y)
 {
	int i, j, k, t, a=0, counter=0, flag=0, count=0, boardSize = state->rows*state->columns, chosenIndice=0, NOP; /*NOP = numOfPossibilities*/
	int **helper = allocateEmptyBoard2d(boardSize);
	for(count=0; count<1000; count++)
	{
		zeroBoard(state->board, boardSize);

		/*randomly filling X cells*/
		for(k=0; k < x; k++){
			do{
				i = rand()%boardSize;
				j = rand()%boardSize;
			}while(state->board[i][j] != 0);

			NOP=0;
			for(t=1; t<=boardSize; t++){
				if (isValidValue(state->board, state->rows, state->columns, i, j, t)){
					NOP++;
				}
			}

			if (NOP == 0){
				flag=1;
				break;

			} else if (NOP==1){ /*no need to choose the value randomly*/
				for(t=1; t<=boardSize; t++){
					if (isValidValue(state->board, state->rows, state->columns, i, j, t)){
						state->board[i][j] = t;
					}
				}

			}else{
				chosenIndice = 1 + rand()%NOP;
				for(t=1;chosenIndice>0; t++){
					if (isValidValue(state->board, state->rows, state->columns, i, j, t)){
						chosenIndice--;
					}
				}
				t--; /*to counteract the t++ before checking the condition*/
				state->board[i][j] = t;
			}
		}
		if (!flag){ /*if: we set X cells successfully*/
			a = ILPsolver(state);
			if (a == 1){
				break; /*an ILP solution was found in fewer than 1000 tries*/
			}
		}
	}
	if (a != 1){ /*if we left the for by trying 1000 times rather than breaking*/
		return 0;
	}
	/*choosing y random cells to clear the others*/
	for(k=0; k<y; k++){
		do{
			i = rand()%boardSize;
			j = rand()%boardSize;
		}while(state->board[i][j] < 0);
		state->board[i][j] = -state->board[i][j]; /*fixing the cell*/
	}
	for(i=0; i<boardSize; i++){
		for(j=0; j<boardSize; j++){
			if (state->board[i][j] > 0){
				state->board[i][j] = 0;
			}
		}
	}
	for(i=0; i<boardSize; i++){
		for(j=0; j<boardSize; j++){
			if(state->board[i][j] < 0){
				state->board[i][j] = -state->board[i][j];
			}
		}
	}
	mem2dcpy(helper, state->board, boardSize);
	zeroBoard(state->board, boardSize);
	for(i=0; i<boardSize; i++){
		for(j=0; j<boardSize; j++){
			if(helper[i][j] != 0){
				incrementUndo(state, i, j, helper[i][j]);
				counter++;
				state->currentNode->mulSetIndex = counter;
			}
		}
	}
	free2dArray(state->board, boardSize);
	free(state->board);
	state->board = helper;
	/*freeDLLBeyond(state->bottomNode)*/;/*if (and only if) the generate succeeds, we need to empty the undo list*/
	return 1;
 }

/*
 * finds he number of solutions using a stack, as dictated by the final project pdf
 *
 *  pre: state.board does not contain erroneous values
 */
int getNumOfSolutions(gameState state)
{
	int boardSize = state.rows*state.columns, i, j, numOfSolutions=0, flag=0; /*flag ==  1 <-> we just previous loop found a solution*/
	DLLNode* head;
	int** newBoard;
	newBoard= calloc(boardSize, sizeof(int*)); /*so that we could corrupt the board values*/
	for(i=0; i<boardSize; i++){
		newBoard[i] = calloc(boardSize ,sizeof(int));
		memcpy(newBoard[i], state.board[i], boardSize*sizeof(int));
	}
	fixAllValues(newBoard, boardSize);

	head = (DLLNode*)malloc(sizeof(DLLNode));
	head->row = 0; 	head->column = 0;  head->prevV = 0;  head->postV = 0;	head->prevN = NULL; head->nextN = NULL;

	while(1){
		for(i=0; i<boardSize; i++){
			for(j=0; j<boardSize; j++){
				if (flag){
					i = head->row;	j = head->column; /*otherwise whenever we find a solution we would start again from [0][0]*/
					flag = 0;

				} else if (newBoard[i][j]<0){
					continue;

				} else if (newBoard[i][j]==0){
					head->nextN = (DLLNode*)malloc(sizeof(DLLNode));
					head->nextN->row = i;	head->nextN->column = j; 	head->nextN->prevV = 0;  head->nextN->postV = 0;
					head->nextN->prevN = head; head->nextN->nextN = NULL;
					head = head->nextN;
				}

				do{
					head->postV++;
				}while((head->postV <=boardSize) && !isValidValue(newBoard, state.rows, state.columns, i, j, head->postV)); /*finds the next valid value for [i][j]*/

				if (head->postV <= boardSize){
					newBoard[i][j] = head->postV;

				} else if (head->prevN->prevN == NULL){ /*doesn't try to access an unallocated value because there's the 0 bottom node and because to get here the stack must've been appended at least once */
					free2dArray(newBoard, boardSize);
					free(newBoard);
					head = head->prevN;
					free(head->nextN);
					head->nextN = NULL;
					free(head);

					return numOfSolutions;

				}else{
					newBoard[i][j] = 0;
					head = head->prevN;
					free(head->nextN);
					head->nextN = NULL;
					i = head->row; j = head->column; /*going back to the previous non-fixed cell*/
					j--; /*to counteract the j++ in the inner loop*/
				}
			}
		}
		/*end of for. here newBoard represents a solution and the stack is full*/
		numOfSolutions++;
		flag = 1;
	}
	return -1; /*unreachable yet needed to satisfy the compiler*/
}


/*
 * goes over every empty cell and checks if there is only one possible value, and if so, fills it, and adds the appropriate link to the DLL.
 * the links added by autofill have an ascending autofillIndex value
 * pre: the board does not contain erroneous values.
 */
void autofillBoard(gameState* state)
{
	int i, j, v, boardSize = state->rows*state->columns, counter=0;
	int** board; /*the board that gets updated while we check legality using board.state to prevent cell [4,5] effecting [5,5]*/
	board = allocateEmptyBoard2d(boardSize);
	mem2dcpy(board, state->board, boardSize);
	for(i=0; i<boardSize; i++){
		for(j=0; j<boardSize; j++){
			if (state->board[i][j]==0){
				for(v=1; v<=boardSize; v++){
					if (isValidValue(state->board, state->rows, state->columns, i, j, v)){
						if (board[i][j]==0){
							board[i][j] = v;

						}else{
							board[i][j] = 0;
							break;
						}
					}
				}
				/*here board[i][j] is 0 if there were multiple legal values and the value if singular*/
				if (board[i][j]!=0){ /*the first time incrementUndo is called (if at all) the redo nodes will get cleared*/
					incrementUndo(state, i, j, board[i][j]);
					printf("Cell <%d,%d> set to %d\n", j+1, i+1, board[i][j]); /*intentionally reversed i and j to fit the <column,row> format*/
					counter++;
					state->currentNode->autofillIndex = counter;
					 /*so the first node added by the autofill has index=1, the next 2... this is so that in undoMove and
					   redoMove we could undo/redo them all at once, and tell apart the nodes from two different autofills*/
				}
			}
		}
	}
	free2dArray(state->board, boardSize);
	free(state->board);
	state->board = board;


}

/*
 * allocates an empty board, for when the command edit [no path] is called, and frees old fields
 */
void editNewBoard(gameState* state, int rows, int columns){
	int** newBoard;

	freeStateExecptBottomNode(state);

	state->rows = rows;
	state->columns = columns;
	newBoard = allocateEmptyBoard2d(rows*columns);

	state->board = newBoard;
}


/*
 * adds a node to the undo/redo list, and if the current node isn't the last one
 * also frees all the nodes that were ahead
 */
void incrementUndo(gameState* state, int row, int column, int newV)
{
	DLLNode* next = (DLLNode*)malloc(sizeof(DLLNode));
	next->row = row;
	next->column = column;
	next->postV = newV;
	next->prevV = state->board[row][column];
	next->nextN = NULL;
	next->prevN = (state->currentNode);
	next->autofillIndex = 0;

	if (state->currentNode->nextN){
		freeDLLBeyond(state->currentNode);
	}
	state->currentNode->nextN = next;
	state->currentNode = next;
}

/*
 * undoes the move represented by currentNode, and sets currentNode to the previous one. prints as needed
 * pre:currentNode!=bottomNode
 */
void undoMove(gameState* state)
{
	DLLNode* currentBackup; /*for post undoing*/
	do{
	state->board[state->currentNode->row][state->currentNode->column] = state->currentNode->prevV;
	state->currentNode = state->currentNode->prevN;
	}while((state->currentNode->autofillIndex!=0 || state->currentNode->mulSetIndex!=0) &&		/*while: current node is part of an autofill or generatePuzzle &&*/
			(state->currentNode->autofillIndex < state->currentNode->nextN->autofillIndex || state->currentNode->mulSetIndex < state->currentNode->nextN->mulSetIndex));  /*it's part of the same autofill as the one we just undid*/

	/*the last check is represented correctly in the code because in the autofill we give them indices 1,2,3.. so while undoing the same autofill currentIndex<nextIndex
	 * but between different autofills we get currentIndex>=1 and nextIndex=1 so currentIndex>=nextIndex */

	printBoardFromState(*state);
	currentBackup = state->currentNode;
	/*printing the messages, and if it was an autofill, in order rather than reverse order*/
	do{
		state->currentNode = state->currentNode->nextN;
		undoRedoMessage(state, 1);
	}while(state->currentNode->nextN && 													/*while: there are more nodes in the list 		 &&*/
			(state->currentNode->autofillIndex!=0 || state->currentNode->mulSetIndex!=0)&& 										/*the last link we undid was part of an autofill or generatePuzzle &&*/
			(state->currentNode->autofillIndex < state->currentNode->nextN->autofillIndex || state->currentNode->mulSetIndex < state->currentNode->nextN->mulSetIndex));  /*the next link is part of the same autofill or generatePuzzle*/

	state->currentNode = currentBackup;
}

/*
 * redoes the move represented by the node after currentNode, and set currentNode to be it. prints as needed
 * pre: currentNode->nextN != NULL
 */
void redoMove(gameState* state)
{
	DLLNode* currentBackup; /*for pre-redoing*/
	currentBackup = state->currentNode;
	do{
	state->currentNode = state->currentNode->nextN;
	state->board[state->currentNode->row][state->currentNode->column] = state->currentNode->postV;
	}while((state->currentNode->autofillIndex!=0 || state->currentNode->mulSetIndex!=0) && 										   /*while: the node we redone was part of an autofill &&*/
			state->currentNode->nextN &&		  										   /*there is a next node &&*/
			(state->currentNode->autofillIndex < state->currentNode->nextN->autofillIndex || state->currentNode->mulSetIndex < state->currentNode->nextN->mulSetIndex)); /*the next node is also part of the same autofill (otherwise nextIndex = 0 or 1, but currentIndex>0)*/

	printBoardFromState(*state);
	state->currentNode = currentBackup;
	/*printing the messages*/
	do{
	state->currentNode = state->currentNode->nextN;
	undoRedoMessage(state, 0);
	}while((state->currentNode->autofillIndex!=0 || state->currentNode->mulSetIndex!=0) && /*same while as above. the lines that differ inside the loop don't*/
			state->currentNode->nextN &&		   /*effect the parameters the while checks so it ends at the same link*/
			(state->currentNode->autofillIndex < state->currentNode->nextN->autofillIndex || state->currentNode->mulSetIndex < state->currentNode->nextN->mulSetIndex));


}
