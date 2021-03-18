/*
 * ILPsolver.c
 *
 *  the source file used with ILPsolver.h
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "ILPsolver.h"
#include "auxiliary.h"
#include "gurobi_c.h"


/* This function uses the following model for solving sudoku:

     maximize    0
     subject to
          sum_v x[i,j,v] = 1
          sum_i x[i,j,v] = 1
          sum_j x[i,j,v] = 1
          sum_block x[i,j,v] = 1

		  as well as x[i,j,v]=1 for every cell [i,j] with a value v



   this function accepts a state and solve its board.
   it returns 1 if the board is successfully solved or 0 if there is no solution. it returns -1 in any different case.
*/

int ILPsolver(gameState* state)
{

  GRBenv    *env   = NULL;
  GRBmodel  *model = NULL;
  int       i=0, j=0, v, ii, jj, tmp, index, m=state->rows, n=state->columns;
  int       error = 0, boardSize=m*n, **backupBoard = allocateEmptyBoard2d(boardSize);
  int       *ind = calloc(boardSize, sizeof(int));
  double    *val = calloc(boardSize, sizeof(double));
  double    *sol = calloc(boardSize*boardSize*boardSize, sizeof(double));
  int       optimstatus;
  char      *vtype = calloc(boardSize*boardSize*boardSize, sizeof(char));


  for(i=0; i<boardSize; i++){
	  for(j=0; j<boardSize; j++){
		  if(state->board[i][j] < 0){
			  backupBoard[i][j] = -1;
		  }
	  }
  }

  /* Create environment */

  error = GRBloadenv(&env, "sudoku.log");
  if (error) {
	free2dArray(backupBoard, boardSize);
	free(backupBoard);
	free(ind);
	free(val);
	free(sol);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
 	return -1;
   }

  /*setting the outputFlag to 0 which prevents gurobi from printing to screen*/

  error = GRBsetintparam(env, "OutputFlag", 0);
  if(error){
	free2dArray(backupBoard, boardSize);
	free(backupBoard);
	free(ind);
	free(val);
	free(sol);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
	return -1;
  }


  /* Create new model */

  error = GRBnewmodel(env, &model, "sudoku", 0,NULL, NULL, NULL, NULL, NULL);
  if (error) {
	free2dArray(backupBoard, boardSize);
	free(backupBoard);
	free(ind);
	free(val);
	free(sol);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
 	return -1;
   }

  /*define what kind the variables are*/
  for (i = 0; i < boardSize; i++) {
    for (j = 0; j < boardSize; j++) {
      for (v = 0; v < boardSize; v++) {
        vtype[i*boardSize*boardSize+j*boardSize+v] = GRB_BINARY;
      }
    }
  }

  /* add variables to model */

  error = GRBaddvars(model, boardSize*boardSize*boardSize, 0, NULL, NULL, NULL, NULL, NULL, NULL, vtype, NULL);
  if (error) {
	free2dArray(backupBoard, boardSize);
	free(backupBoard);
	free(ind);
	free(val);
	free(sol);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
	return -1;
  }


  /* Change objective sense to maximization */
  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
  if (error) {
	free2dArray(backupBoard, boardSize);
	free(backupBoard);
	free(ind);
	free(val);
	free(sol);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
	return -1;
  }


  /* update the model - to integrate new variables */

  error = GRBupdatemodel(model);
  if (error) {
	free2dArray(backupBoard, boardSize);
	free(backupBoard);
	free(ind);
	free(val);
	free(sol);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
	return -1;
  }


  /* Each cell gets a single value */

  for (i = 0; i < boardSize; i++) {
	  for (j = 0; j < boardSize; j++) {
	      for (v = 0; v < boardSize; v++) {
	        ind[v] = i*boardSize*boardSize + j*boardSize + v;
	        val[v] = 1.0;
	      }
	      error = GRBaddconstr(model, boardSize, ind, val, GRB_EQUAL, 1.0, NULL);
	      if (error) {
		  free2dArray(backupBoard, boardSize);
		  free(backupBoard);
		  free(ind);
		  free(val);
		  free(sol);
		  free(vtype);
		  GRBfreemodel(model);
		  GRBfreeenv(env);
	    	  return -1;
	      }
	  }
  }

  /* Each value appears once in each row */

  for (v = 0; v < boardSize; v++) {
	  for (j = 0; j < boardSize; j++) {
	      for (i = 0; i < boardSize; i++) {
	        ind[i] = i*boardSize*boardSize + j*boardSize + v;
	        val[i] = 1.0;
	      }
	      error = GRBaddconstr(model, boardSize, ind, val, GRB_EQUAL, 1.0, NULL);
	      if (error) {
		  free2dArray(backupBoard, boardSize);
		  free(backupBoard);
		  free(ind);
		  free(val);
		  free(sol);
		  free(vtype);
		  GRBfreemodel(model);
		  GRBfreeenv(env);
	    	  return -1;
	      }
	  }
  }

  /* Each value appears once in each column */

  for (v = 0; v < boardSize; v++) {
	  for (i = 0; i < boardSize; i++) {
	      for (j = 0; j < boardSize; j++) {
	        ind[j] = i*boardSize*boardSize + j*boardSize + v;
	        val[j] = 1.0;
	      }
	      error = GRBaddconstr(model, boardSize, ind, val, GRB_EQUAL, 1.0, NULL);
	      if (error) {
		  free2dArray(backupBoard, boardSize);
		  free(backupBoard);
		  free(ind);
		  free(val);
		  free(sol);
		  free(vtype);
		  GRBfreemodel(model);
		  GRBfreeenv(env);
	    	  return -1;
	      }
	  }
  }

  /* Each value appears once in each block */

  for (v = 0; v < boardSize; v++) {
	  for (ii = 0; ii < n; ii++) {
	      for (jj = 0; jj < m; jj++) {
	        index = 0;
	        for (i = ii*m; i < (ii+1)*m; i++) {
	          for (j = jj*n; j < (jj+1)*n; j++) {
	            ind[index] = i*boardSize*boardSize + j*boardSize + v;
	            val[index] = 1.0;
	            index++;
	          }
	        }
	        error = GRBaddconstr(model, boardSize, ind, val, GRB_EQUAL, 1.0, NULL);
	        if (error) {
		  free2dArray(backupBoard, boardSize);
		  free(backupBoard);
		  free(ind);
		  free(val);
		  free(sol);
		  free(vtype);
		  GRBfreemodel(model);
		  GRBfreeenv(env);
	      	  return -1;
	        }
	      }
	  }
  }

  /*each non-empty cell [i][j] in state.board means the variable x[i,j,v] equals 1.0*/

  for(i=0; i<boardSize; i++){
	  for(j=0; j<boardSize; j++){
		  if(state->board[i][j] != 0){
			  	tmp = ((state->board[i][j]>0)? state->board[i][j]:-state->board[i][j]); /* = absoluteValue(state.board[i][j]). added because a fixed v is implemented as -v*/
		        ind[0] = i*boardSize*boardSize + j*boardSize + tmp -1;
		        val[0] = 1.0;
		        error = GRBaddconstr(model, 1, ind, val, GRB_EQUAL, 1.0, NULL);
		        if (error) {
				free2dArray(backupBoard, boardSize);
				free(backupBoard);
				free(ind);
				free(val);
				free(sol);
				free(vtype);
				GRBfreemodel(model);
				GRBfreeenv(env);
		  		return -1;
		        }
		  }
	  }
  }


  /* Optimize model - need to call this before calculation */
  error = GRBoptimize(model);
  if (error) {
	free2dArray(backupBoard, boardSize);
	free(backupBoard);
	free(ind);
	free(val);
	free(sol);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
	return -1;
  }

  /* Get solution information */

   error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
   if (error) {
	free2dArray(backupBoard, boardSize);
	free(backupBoard);
	free(ind);
	free(val);
	free(sol);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
 	return -1;
   }

  /* get the solution - the assignment to each variable */
  /* Dim*Dim*Dim number of variables, the size of "sol" should match */

  error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, boardSize*boardSize*boardSize, sol);
  if (error) {
	free2dArray(backupBoard, boardSize);
	free(backupBoard);
	free(ind);
	free(val);
	free(sol);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
	return -1;
  }

  /* solution found */
  if (optimstatus == GRB_OPTIMAL) {
	  for(i=0; i<boardSize; i++){
		  for(j=0; j<boardSize; j++){
			  for(v=0; v<boardSize; v++){
				  if(sol[i*boardSize*boardSize+j*boardSize+v]==1.0){
					  state->board[i][j] = ((backupBoard[i][j] == -1) ? (v+1)*(-1) : v+1);
				  }
			  }
		  }
	  }
	free2dArray(backupBoard, boardSize);
	free(backupBoard);
	free(ind);
	free(val);
	free(sol);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
	return 1;
  }

  /* no solution found */
  else if (optimstatus == GRB_INF_OR_UNBD) {
	free2dArray(backupBoard, boardSize);
	free(backupBoard);
	free(ind);
	free(val);
	free(sol);
	free(vtype);
    GRBfreemodel(model);
    GRBfreeenv(env);
    return 0;
  }

  /* error or calculation stopped */
  else {
    printf("Optimization was stopped early\n");
	free2dArray(backupBoard, boardSize);
	free(backupBoard);
	free(ind);
	free(val);
	free(sol);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
	return -1;
  }
  return -1;
}


/*
 * receives a state and returns 1 if it's board is solvable, 0 if its not and -1 on errors.
 * calls ILPsolver.
 * this function is used when the board shouldn't be corrupted (solved)
 */
int isSolvableBoard(gameState* state)
{
	int answer, boardSize = state->rows*state->columns;
	int **backupBoard = allocateEmptyBoard2d(boardSize);
	mem2dcpy(backupBoard, state->board, boardSize);

	answer = ILPsolver(state);

	free2dArray(state->board, boardSize);
	free(state->board);
	state->board = backupBoard; /*the original board is solved by the ILPsolver, so we change to the backup to retain the original board state*/
	return answer;
}

