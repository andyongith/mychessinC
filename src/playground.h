#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <stdbool.h>

#include "position.h"

extern int showlegals;
extern int botmvdelay;
extern bool enable_unicode;

void play_randomly(board_t board);
void play_manually(int side, board_t board);

int askPromotionPiece();

#endif // PLAYGROUND_H
