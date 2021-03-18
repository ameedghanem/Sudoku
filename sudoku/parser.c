/*
 * parser.c
 *
 *  the source file used with parser.h
 */
#include <string.h>
#include <stdio.h>
#include<stdlib.h>
#include "parser.h"
#include "auxiliary.h"

/*
 * turns a string of a command name into the corresponding COMMAND enumerator value
 */
COMMAND COMMAND_translator(char* command){

	if(!strcmp(command,"solve"))				{ return solve;}

	else if(!strcmp(command,"edit"))			{ return edit;}

	else if(!strcmp(command,"mark_errors"))		{ return mark_errors;}

	else if(!strcmp(command,"print_board"))		{ return print_board;}

	else if(!strcmp(command,"pb"))				{ return print_board;}/*to remove before submitting, for laziness' sake*/

	else if(!strcmp(command,"set"))				{ return set;}

	else if(!strcmp(command,"validate"))		{ return validate;}

	else if(!strcmp(command,"generate"))		{ return generate;}

	else if(!strcmp(command,"undo"))			{ return undo;}

	else if(!strcmp(command,"redo"))			{ return redo;}

	else if(!strcmp(command,"save"))			{ return save;}

	else if(!strcmp(command,"hint"))			{ return hint;}

	else if(!strcmp(command,"num_solutions"))	{ return num_solutions;}

	else if(!strcmp(command,"autofill"))		{ return autofill;}

	else if(!strcmp(command,"reset"))			{ return reset;}

	else if(!strcmp(command,"exit"))			{ return exit_command;}

	else 										{ return invalid_command;}
}

/*
 * accepts a string and a commandBlock
 * translates the command in the string into commandBlock form and modifies the commandBlock given accordingly
 * as per instructions, any input beyond the tokens necessary for the command are ignored
 */
void commandParser(char *command, commandBlock* block){
	int i=0, *intComp=NULL, intCompCount=0; /*int component, such as 5 6 7 for set*/
	double wholenessChecker;
	char *tokens=NULL, mycommand[256]={0},  *delimiter = " \t\r\n" ;
	block->integerComponent = intComp;
	tokens = strtok(command, delimiter);
	strcpy(mycommand, tokens);

	block->command = COMMAND_translator(mycommand);

	if(block->command == solve || block->command == edit || block->command == save ){ /*the cases in which we expect a path rather than integers*/

		block->path = strtok(NULL, delimiter);
		if(block->path==NULL && block->command != edit){ /*the only path command where the path is optional is edit*/
				block->command = invalid_command;
		}
		return;
	}

	if(block->command != mark_errors && block->command != set && block->command != generate && block->command != hint){
		return; /*if there is no int component to the command*/
	}

	intComp = calloc(1024, sizeof(int));
	for(i=0; i<1024; i++){
		intComp[i]=-1;
	}

	i=0;
	while((command = strtok(NULL, delimiter)) != NULL){
		wholenessChecker = atof(command);
		if(wholenessChecker/1 != wholenessChecker){ /*if input wasn't a whole number*/
			intComp[i] = -2; /*-2 was chosen so it would not be -1 (which the program considers unentered) but will be out of range*/

		}else{
			intComp[i] = (int)wholenessChecker;
		}
		if(intComp[i] == -1){ /*-1 is never a valid input for any command, however the error should be 'out of range 0-N' rather than */
			intComp[i] = -2;  /*'invalid command' due to the mistaken belief by the program that there weren't inputed enough variables*/
		}
		i++;
	}
	block->integerComponent = intComp;

	if(block->command == mark_errors)								{ intCompCount = 1;}

	else if(block->command == set)									{ intCompCount = 3;}

	else if(block->command == generate || block->command == hint) 	{ intCompCount = 2;} /*one of these always occurs, but it's an else if for clarity*/


	for(i=0; i<intCompCount; i++){
		if(intComp[i] == -1){
			block->command = invalid_command; /*not enough int inputs*/
			free(intComp); /*an invalid command doesn't require int components*/
			return;
		}
	}
}

/*
 * frees the innards of a command block.
 * note it does not free the block itself.
 */
void freeCommandBlock(commandBlock* block)
{
	if (block->integerComponent != NULL){
		free(block->integerComponent);
	}

	/*if (block->path != NULL){ //path is never allocated dynamically so it's freed automatically
		free(block->path);
	}*/
}
