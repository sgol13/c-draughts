// Draughts in terminal / C, XML *
// * * * * * * * * * * * * * * * *

// Interface of the game.
// Functions which display output and read input.

#ifndef INTERFACE_H
#define INTERFACE_H

#include "defs.h"

void cleanScreen();

void cleanBuffer();

void setColor(char *color);

int CTI(char x);

bool checkRange(int x, int game_size);

void printEndGame(int turn);

void printBoard(int board[12][12], char *top_pattern, char *row_pattern, int game_size);

void loadBoardPattern(int game_size, char **top_pattern, char **row_pattern);

int readInput(int *from_x, int *from_y, int *to_x, int *to_y, int game_size);

void printManual();

void printInputLine(int turn);


#endif
