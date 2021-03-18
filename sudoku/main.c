/*
 * main.c
 *
 *  Created on: May 19, 2018
 *      Author: user
 */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include "auxiliary.h"
#include "game.h"
#include "parser.h"
#include "ILPsolver.h"
#include "printer.h"

int main()
{
	char commandStream[256]= {0}; /*there's no need to free constantly sized arrays*/
	commandBlock command;
	GAMEMODE mode = init_mode;
	gameState state;
	DLLNode* bottom = (DLLNode*)malloc(sizeof(DLLNode));
	bottom->row = 0;	  bottom->column = 0;	bottom->prevV = 0;	bottom->postV = 0;	bottom->nextN = NULL;	bottom->prevN = NULL; bottom->autofillIndex = 0;
	/*we use this node as the unchanging basis of the stack, even when changing puzzles*/
	state.bottomNode = bottom;
	state.currentNode = bottom;
	state.mode = mode;
	state.markErrors = 1;
	state.board = NULL;
	srand(time(NULL));
	messagePrinter(sudoku);
	setbuf(stdout, NULL);
	do{
		printf("Enter your command:\n");
		fgets(commandStream, 1024, stdin);
		commandParser(commandStream, &command);
		setCommands(&state, command);
	}while(command.command != exit_command);
	freeState(&state);			/*we don't need to free state itself since state IS the list */
	freeCommandBlock(&command); /*of pointers we free in freeState() and so are freed already*/
	return -1;

}



