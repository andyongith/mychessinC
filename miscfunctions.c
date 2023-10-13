
#include "position.h"
#include "miscfunctions.h"

int colorofpiece(int piece) {
  return piece & 24; // 24 = WHITE | BLACK
}

int typeofpiece(int piece) {
  return piece & 7; // 7 = KING | QUEEN | KNIGHT | ROOK | BISHOP
}

int oppositecolor(int color) {
  return color ^ 24; // 24 = WHITE | BLACK
}
