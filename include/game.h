// Draughts in terminal / C, XML *
// * * * * * * * * * * * * * * * *


#ifndef GAME_H
#define GAME_H

#include "defs.h"

void game(int game_size, bool new_game);

void initialGameSet(int board[12][12], int game_size);

void saveGame(int board[12][12], int game_size, int turn);

void loadGame(int board[12][12], int *game_size, int *turn);

bool isPossible(int board[12][12], int turn, int from_x, int from_y, int to_x,
                int to_y, int pr_x, int pr_y);

int countPieces(int board[12][12], int player, int game_size);

bool move(int board[12][12], int turn, int from_x, int from_y, int to_x, int to_y);

int OP(int turn);

bool isOpposite(int x, int turn);

bool isSame(int x, int turn);

bool checkCapture(int board[12][12], int x, int y, int turn, int game_size);


#endif
