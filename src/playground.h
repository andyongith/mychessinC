#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include "position.h"

extern int showlegals;
extern int botmvdelay;

void play_randomly(board_t board);
void play_manually(int side, board_t board);

int askPromotionPiece();

#endif // PLAYGROUND_H
