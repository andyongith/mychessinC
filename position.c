#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "position.h"

char STARTFEN[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

struct __Board__ Board;
uint8_t piece_sym['z']; // Piece Representations

void initPositionVars() {
  for(int i=0; i<'a'; i++) piece_sym[i] = ' ';
  for(int i='a'; i<'z'; i++) piece_sym[i] = 0;

  piece_sym['k'] = BLACK | KING;
  piece_sym['p'] = BLACK | PAWN;
  piece_sym['n'] = BLACK | KNIGHT;
  piece_sym['b'] = BLACK | BISHOP;
  piece_sym['r'] = BLACK | ROOK;
  piece_sym['q'] = BLACK | QUEEN;
  piece_sym['K'] = WHITE | KING;
  piece_sym['P'] = WHITE | PAWN;
  piece_sym['N'] = WHITE | KNIGHT;
  piece_sym['B'] = WHITE | BISHOP;
  piece_sym['R'] = WHITE | ROOK;
  piece_sym['Q'] = WHITE | QUEEN;
  
  piece_sym[BLACK | KING]  = 'k';
  piece_sym[BLACK | PAWN]  = 'p';
  piece_sym[BLACK | KNIGHT]= 'n';
  piece_sym[BLACK | BISHOP]= 'b';
  piece_sym[BLACK | ROOK]  = 'r';
  piece_sym[BLACK | QUEEN] = 'q';
  piece_sym[WHITE | KING]  = 'K';
  piece_sym[WHITE | PAWN]  = 'P';
  piece_sym[WHITE | KNIGHT]= 'N';
  piece_sym[WHITE | BISHOP]= 'B';
  piece_sym[WHITE | ROOK]  = 'R';
  piece_sym[WHITE | QUEEN] = 'Q';

  for(int i=0; i<64; i++) Board.square[i] = 0;
  Board.turn = WHITE;
  Board.castle = 0;
  Board.en_passant_pawn=-1;
  Board.fen[0] = '\0';
}

void setPosition(char* fen) {
  int fenlen = strlen(fen);
  
  int i=0;
  // Board Setup
  int row=7, col=0;
  while(i<fenlen && fen[i]!=' ') {
    if(fen[i]=='/') { row--; col=0; }
    else if( '0'<=fen[i] && fen[i]<='9' ) {
      int skipcol = (int) (fen[i]-'0');
      while(skipcol--) { Board.square[row*8 + col]=0; col++; }
    }
    else { Board.square[row*8 + col] = piece_sym[fen[i]]; col++; } // needs attention
    i++;
  }

  while(i<fenlen && fen[i]==' ') i++;

  // Others
  Board.turn = fen[i]=='w' ? WHITE : BLACK;
  
}

void removeSquare(int sqr) {
  Board.square[sqr] = NOPIECE;
}

void shiftSquare(int from, int to) {
  Board.square[to] = Board.square[from];
  Board.square[from] = NOPIECE;
}

void putSquare(int piece, int sqr) {
  Board.square[sqr] = piece;
}

void changeTurn() {
  Board.turn = Board.turn == WHITE ? BLACK : WHITE;
}

int currentTurn() {
  return Board.turn;
}

void updateEnPassantPawn(int sqr) {
  Board.en_passant_pawn = sqr;
}

void removeEnPassantPawn() {
  Board.en_passant_pawn = -1;
}

// Only For debugging(will remove it later)(dont' forget to also remove from header)
void printSquares() {
  for(int i=0; i<64; i++) {
    printf("%c,", piece_sym[Board.square[i]]);
  }
}
