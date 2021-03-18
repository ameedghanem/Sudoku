/*
 * ILPsolver.h
 *
 *  this header and source hold the functions that use the gurobi library for ILP.
 *  info about individual functions can be found before them in the source
 */

#ifndef ILPSOLVER_H_
#define ILPSOLVER_H_

#include "game_logic.h"

#endif /* ILPSOLVER_H_ */


/*ILP functions*/
int ILPsolver(gameState* state);
int isSolvableBoard(gameState* state);
