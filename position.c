#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "position.h"

uint8_t piece_sym['z']; // Piece Representations

void init_board(board_t* board) {
  for(int i=0; i<64; i++) board->square[i] = NOPIECE;
  board->turn = WHITE;
  board->castle = 0;
  board->en_passant_pawn=-1;
  board->fen[0] = '\0';
}

void initPositionVars(board_t* board) {
  for(int i=0; i<'a'; i++) piece_sym[i] = ' ';
  for(int i='a'; i<'z'; i++) piece_sym[i] = 0;

  int letters1[] = {'k','p','n','b','r','q'};
  int letters2[] = {'K','P','N','B','R','Q'};
  int values[] = {KING, PAWN, KNIGHT, BISHOP, ROOK, QUEEN};
  for(int i=0; i<6; i++) piece_sym[letters1[i]] = BLACK | values[i];
  for(int i=0; i<6; i++) piece_sym[letters2[i]] = WHITE | values[i];
  for(int i=0; i<6; i++) piece_sym[BLACK | values[i]] = letters1[i];
  for(int i=0; i<6; i++) piece_sym[WHITE | values[i]] = letters2[i];
  piece_sym[' '] = NOPIECE;
  piece_sym[NOPIECE] = ' ';

  init_board(board);
}

void setPosition(char* fen, board_t* board) {
  int fenlen = strlen(fen);
  
  int i=0;
  // Board Setup
  int row=7, col=0;
  while(i<fenlen && fen[i]!=' ') {
    if(fen[i]=='/') { row--; col=0; }
    else if( '0'<=fen[i] && fen[i]<='9' ) {
      int skipcol = (int) (fen[i]-'0');
      while(skipcol--) { board->square[row*8 + col]=0; col++; }
    }
    else { board->square[row*8 + col] = piece_sym[fen[i]]; col++; }
    i++;
  }
  while(i<fenlen && fen[i]==' ') i++;

  // turn
  board->turn = fen[i++]=='w' ? WHITE : BLACK;
  while(i<fenlen && fen[i]==' ') i++;

  // castling
  board->castle = 0;
  while(i<fenlen && fen[i]!=' ') {
    switch(fen[i++]) {
      case 'K': board->castle |= WHITE_KING_SIDE ; break; 
      case 'Q': board->castle |= WHITE_QUEEN_SIDE; break;
      case 'k': board->castle |= BLACK_KING_SIDE ; break;
      case 'q': board->castle |= BLACK_QUEEN_SIDE; break;
    }
  }
}

void delPiecefrom(int sqr, board_t* board) { board->square[sqr] = NOPIECE; }
void shiftPiece(int from, int to, board_t* board) {
  board->square[to] = board->square[from];
  board->square[from] = NOPIECE;
}
void setPieceto(int piece, int sqr, board_t* board) { board->square[sqr] = piece; }
void switchTurn(board_t* board) { board->turn = board->turn == WHITE ? BLACK : WHITE; }
int  getTurn(board_t* board) { return board->turn; }
void setEnPassantPawn(int sqr, board_t* board) { board->en_passant_pawn = sqr; }
void delEnPassantPawn(board_t* board) { board->en_passant_pawn = -1; }
void delCastleAbility(int identity, board_t* board) { board->castle &= ~identity; }
