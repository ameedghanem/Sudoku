CC = gcc
OBJS = main.o auxiliary.o ILPsolver.o game_logic.o game.o parser.o printer.o DLL.o file_manager.o
EXEC = sudoku
COMP_FLAGS = -ansi -Wall -Wextra -Werror \
-pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56


$(EXEC): $(OBJS)
		$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c auxiliary.c ILPsolver.h game.h parser.h DLL.h file_manager.h game_logic.h printer.h
		$(CC) $(COMP_FLAGS) -c $*.c
parser.o: parser.c parser.h auxiliary.h
		$(CC) $(COMP_FLAGS) -c $*.c
auxiliry.o: auxiliary.c auxiliary.h game_logic.h
		$(CC) $(COMP_FLAGS) -c $*.c
game_logic.o: game_logic.c game_logic.h auxiliary.h ILPsolver.h printer.h
		$(CC) $(COMP_FLAGS) -c $*.c
ILPsolver.o: ILPsolver.c ILPsolver.h auxiliary.h game_logic.h
		$(CC) $(COMP_FLAGS) $(GUROBI_COMP) $(GUROBI_LIB) -c $*.c
game.o: game.c game.h ILPsolver.h auxiliary.h printer.h file_manager.h
		$(CC) $(COMP_FLAGS) -c $*.c
file_manager.o: file_manager.c file_manager.h game_logic.h auxiliary.h
		$(CC) $(COMP_FLAGS) -c $*.c
DLL.o: DLL.c DLL.h
		$(CC) $(COMP_FLAGS) -c $*.c
printer.o: printer.c printer.h auxiliary.h
		$(CC) $(COMP_FLAGS) -c $*.c
all: $(EXEC)
		
clean:
		rm -f *.o $(OBJS $(EXEC)