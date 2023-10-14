#ifndef POSITION_H
#define POSITION_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

extern char STARTFEN[];

enum Piece {
  NOPIECE=0,
  KING=1, PAWN=2, KNIGHT=3, BISHOP=4, ROOK=5, QUEEN=6,
  WHITE=8, BLACK=16
};

struct __Board__ {
  int square[64];
  int turn; // WHITE or BLACK
  uint8_t castle; // bitfield in the format KQkq, for example 1001 means black can castle queen side and white can castle king side only
  int en_passant_pawn;
  char fen[64];
};
extern struct __Board__ Board;

extern uint8_t piece_sym['z']; // Piece Representations or symbols, can be used for parsing fen and displaying current position

void initPositionVars();

void setPosition(char* fen);
void removeSquare(int sqr);
void shiftSquare(int from, int to);
void changeTurn();
int currentTurn();
void updateEnPassantPawn(int sqr);
void removeEnPassantPawn();
void putSquare(int piece, int sqr);
void delCastleAbility(int identity);

void printSquares();

#endif // POSITION_H
