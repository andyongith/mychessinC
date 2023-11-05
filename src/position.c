#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "position.h"
#include "miscfunctions.h"

void init_board(board_t* board) {
  for(int i=0; i<64; i++) board->squares[i] = NOPIECE;
  board->turn = WHITE;
  board->castle = 0;
  board->en_passant_pawn=-1;
  board->fen[0] = '\0';
  board->halfmove = 0;
  board->fullmove = 1;
}

// Piece Representations
uint8_t piece_sym(int index) {
  static bool computed=false;
  static int array['z'];
  if(!computed) {
    computed=true;
    for(int i=0; i<'a'; i++) array[i] = ' ';
    for(int i='a'; i<'z'; i++) array[i] = 0;

    int letters1[] = {'k','p','n','b','r','q'};
    int letters2[] = {'K','P','N','B','R','Q'};
    int values[] = {KING, PAWN, KNIGHT, BISHOP, ROOK, QUEEN};
    for(int i=0; i<6; i++) array[letters1[i]] = BLACK | values[i];
    for(int i=0; i<6; i++) array[letters2[i]] = WHITE | values[i];
    for(int i=0; i<6; i++) array[BLACK | values[i]] = letters1[i];
    for(int i=0; i<6; i++) array[WHITE | values[i]] = letters2[i];
    array[' '] = NOPIECE;
    array[NOPIECE] = ' ';
  }
  return array[index];
}

void validate_castle(board_t* board) {
  int castle_ability=0;
  if(board->squares[4]==(WHITE|KING)) {
    if(board->squares[0]==(WHITE|ROOK)) castle_ability |= WHITE_QUEEN_SIDE;
    if(board->squares[7]==(WHITE|ROOK)) castle_ability |= WHITE_KING_SIDE;
  }
  if(board->squares[60]==(BLACK|KING)) {
    if(board->squares[56]==(BLACK|ROOK)) castle_ability |= BLACK_QUEEN_SIDE;
    if(board->squares[63]==(BLACK|ROOK)) castle_ability |= BLACK_KING_SIDE;
  }
  board->castle &= castle_ability;
}

void setPosition(char* fen, board_t* board) {
  init_board(board);
  int fenlen = strlen(fen);
  
  int i=0;
  // Board Setup
  int row=7, col=0;
  while(i<fenlen && fen[i]!=' ') {
    if(fen[i]=='/') { row--; col=0; }
    else if( '0'<=fen[i] && fen[i]<='9' ) {
      int skipcol = (int) (fen[i]-'0');
      while(skipcol--) { board->squares[row*8 + col]=0; col++; }
    }
    else { board->squares[row*8 + col] = piece_sym(fen[i]); col++; }
    i++;
  }
  while(i<fenlen && fen[i]==' ') i++;

  // turn
  board->turn = fen[i++]=='w' ? WHITE : BLACK;
  while(i<fenlen && fen[i]==' ') i++;

  // castling
  board->castle = 0;
  while(i<fenlen && fen[i]!=' ') {
    if(fen[i]=='-') { i++; break; }
    switch(fen[i++]) {
      case 'K': board->castle |= WHITE_KING_SIDE ; break; 
      case 'Q': board->castle |= WHITE_QUEEN_SIDE; break;
      case 'k': board->castle |= BLACK_KING_SIDE ; break;
      case 'q': board->castle |= BLACK_QUEEN_SIDE; break;
    }
  }
  validate_castle(board);

  // en passant
  while(i<fenlen && fen[i]==' ') i++;
  if(fen[i]=='-') board->en_passant_pawn=-1;
  else board->en_passant_pawn = nameToIndex(fen+i);
  i++;

  // halfmove and fullmove
  board->halfmove = 0;
  board->fullmove = 1;

  while(i<fenlen && fen[i]==' ') i++;
  if(i >= fenlen) return;
  int halfmove = fen[i++] - '0';
  if(fen[i]!=' ') halfmove = halfmove*10 + fen[i++] - '0';
  board->halfmove = halfmove;

  while(i<fenlen && fen[i]==' ') i++;
  if(i >= fenlen) return;
  int fullmove = fen[i++] - '0';
  if(i<fenlen && fen[i]!=' ') fullmove = fullmove*10 + fen[i++] - '0';
  board->fullmove = fullmove;
}

void shiftPiece(int from, int to, board_t* board) {
  board->squares[to] = board->squares[from];
  board->squares[from] = NOPIECE;
}
void switchTurn(board_t* board) { board->turn = board->turn == WHITE ? BLACK : WHITE; }
int  getTurn(board_t* board) { return board->turn; }
void setEnPassantPawn(int sqr, board_t* board) { board->en_passant_pawn = sqr; }
void delEnPassantPawn(board_t* board) { board->en_passant_pawn = -1; }
void delCastleAbility(int identity, board_t* board) { board->castle &= ~identity; }
