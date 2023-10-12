#ifndef POSITION_H
#define POSITION_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

extern char STARTFEN[];

enum Piece {
  KING=1, PAWN=2, KNIGHT=3, BISHOP=4, ROOK=5, QUEEN=6,
  WHITE=8, BLACK=16
};

struct __Board__ {
  int square[64];
  bool turn; //1 for white and 0 for black
  uint8_t castle; //bitfield in the format KQkq, for example 1001 means black can castle queen side and white can castle king side only
  char fen[64];
};
extern struct __Board__ Board;

extern uint8_t piece_sym['z']; // Piece Representations

void initPositionVars();

void setPosition(char* fen);

void printSquares();

#endif // POSITION_H
