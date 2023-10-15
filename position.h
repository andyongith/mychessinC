#ifndef POSITION_H
#define POSITION_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define STARTFEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

enum piece_types {
  NOPIECE=0,
  KING=1, PAWN=2, KNIGHT=3, BISHOP=4, ROOK=5, QUEEN=6,
  WHITE=8, BLACK=16
};

typedef struct {
  int square[64];
  int turn; // WHITE or BLACK
  uint8_t castle; // bitfield in the format KQkq, for example 1001 means black can castle queen side and white can castle king side only
  int en_passant_pawn;
  char fen[64];
} board_t;

extern uint8_t piece_sym['z']; // Piece Representations or symbols, can be used for parsing fen and displaying current position

void init_board(board_t* board);
void initPositionVars(board_t* board);

void setPosition(char* fen, board_t* board);
void delPiecefrom(int sqr, board_t* board);
void shiftPiece(int from, int to, board_t* board);
void switchTurn(board_t* board);
int getTurn(board_t* board);
void setEnPassantPawn(int sqr, board_t* board);
void delEnPassantPawn(board_t* board);
void setPiece(int piece, int sqr, board_t* board);
void delCastleAbility(int identity, board_t* board);

#endif // POSITION_H
