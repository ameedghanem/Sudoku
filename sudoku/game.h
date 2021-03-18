/*
 * game.h
 *
 * this header and source hold the central hub functions of game_logic and printer and for each command an 'MCcommand' which checks that the command is valid **given the state**.
 * the MC functions are all very similar so rather than an individual explanation, they all have a combined one below
 */

#ifndef GAME_H_
#define GAME_H_


#include "parser.h"
#include "DLL.h"
#include "game_logic.h"


#endif /* GAME_H_ */




void setCommands(gameState* state, commandBlock block);


/* MC = modular command. every command has a separate function to not boggle down setCommands
 * the MCs deal with the conditions regarding the commands, although sometimes they do fulfill the commands themselves rather than delegate
 * MCs return 1 if the board should be printed afterwards and 0 otherwise
 */

int MCsolve(gameState* state, char* path);
int MCedit(gameState* state, char* path);
int MCmark_errors(gameState* state, int toMark);
int MCprint_board(gameState* state);
int MCset(gameState* state, int i, int j, int v);
int MCvalidate(gameState* state);
int MCgenerate(gameState* state, int i, int j);
int MCundo(gameState* state);
int MCredo(gameState* state);
int MCsave(gameState* state,char* path);
int MChint(gameState* state, int i, int j);
int MCnum_solutions(gameState* state);
int MCautofill(gameState* state);
int MCreset(gameState* state);
int MCexit();
