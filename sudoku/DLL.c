/*
 * DLL.c
 *
 *  the source file used with DLL.h
 */

#include<stdio.h>
#include<stdlib.h>
#include "DLL.h"

/*
 * freeing all the nodes beyond the one given, not including the one given itself.
 */
void freeDLLBeyond(DLLNode* node)
{
	DLLNode* temp;
	temp = node;
	if (node->nextN == NULL){
		return;
	}
	temp = temp->nextN;
	node->nextN = NULL;
	while(temp->nextN){
		temp = temp->nextN;
		free(temp->prevN); /*the list 1 2 3 4 cleared from the start will be 1 2 3 4 -> 1 3 4 -> 1 4 -> 1*/
	}
	free(temp);
	return;
}

