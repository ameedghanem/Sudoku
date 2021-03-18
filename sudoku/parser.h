/*
 * parser.h
 *
 *  this header and source hold the COMMAND enumerator and the commandBlock struct as well as functions that parse the user input strings and translate them into a commandBlock
 *  info about individual functions can be found before them in the source
 */

#ifndef PARSER_H_
#define PARSER_H_


typedef enum commandType{
	invalid_command,
	solve,
	edit,
	mark_errors,
	print_board,
	set,
	validate,
	generate,
	undo,
	redo,
	save,
	hint,
	num_solutions,
	autofill,
	reset,
	exit_command /*couldn't name it 'exit' because 'exit' already exists in c*/
}COMMAND;


/*
 * as opposed to COMMAND, which merely represents the type, commandBlock represents the entirety of an applicable command, including its additional inputs
 */
typedef struct{
	int* integerComponent;
	char* path;
	COMMAND command;
}commandBlock;

#endif /* PARSER_H_ */


void commandParser(char *command, commandBlock* block);
COMMAND COMMAND_translator(char* command);
void freeCommandBlock(commandBlock* block);

