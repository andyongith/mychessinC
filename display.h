#ifndef DISPLAY_H
#define DISPLAY_H

#include "position.h"
#include "legalmoves.h"

void drawBoard(board_t* board);
void printSquares(board_t* board);
void printLegalMoves(board_t* board, move_t* moves);

#endif // DISPLAY_H
