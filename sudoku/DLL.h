/*
 * DLL.h
 *
 * this header and source hold the DLLNode struct which is used to implement a doubly linked list.
 * functions that regard using the data within the DLL in a given manner (such as undoing a cell) are in game_logic
 */

#ifndef DLL_H_
#define DLL_H_

/*
 * DLLNode represents a single node in a doubly linked list (DLL)
 */

struct DLLNode{
	int row;
	int column;
	int prevV; /*V = value. 0 refers to an empty cell*/
	int postV;
	struct DLLNode* prevN;
	struct DLLNode* nextN;

	int autofillIndex; /*explanation in the autofillBoard function*/
	int mulSetIndex;  /*explanation in the generatePuzzle function*/
};

typedef struct DLLNode DLLNode;



#endif /* DLL_H_ */



void freeDLLBeyond(DLLNode* node);
/*
 * the remaining functions using the stack regard game logic and are therefore in Game
 */

