#ifndef DISPLAY_H
#define DISPLAY_H

#include "position.h"
#include "legalmoves.h"

void drawBoard(board_t board, bool enable_unicode);
void printSquares(board_t board);
void printLegalMoves(move_t* moves);
void printFen(board_t board);

#endif // DISPLAY_H
