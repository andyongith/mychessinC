#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "position.h"
#include "miscfunctions.h"
#include "legalmoves.h"

void addMovebyPosition(int startsqr, int targetsqr, int captures, board_t* board, move_t* move) {
  move->startsqr = startsqr;
  move->targetsqr = targetsqr;
  move->captures = captures;
  move->is_en_passant_pawn = false;
  move->is_castling = false;
  move->promotion = false;
}

// These all functions return no. of moves added
//
int addSlidingPieceMovesto(int sqr, int color, int (*dir)[2], int len, board_t* board, move_t* moves) {
  int mv=0;
  int row=sqr/8, col=sqr%8;
  int oppColor = oppositecolor(color);

  for(int i=0; i<len; i++) {
    for( int r=row+dir[i][0], c=col+dir[i][1];
        0<=r && r<8 && 0<=c && c<8;
        r+=dir[i][0], c+=dir[i][1] )
    {
      int index = r*8 + c;
      int targetcolor = colorofpiece(board->square[index]);
      if(color == targetcolor) break;
      addMovebyPosition(sqr, index, board->square[index]>0 ? index : -1, board, moves + mv);
      mv++;
      if(oppColor == targetcolor) break;
    }
  }

  return mv;
}

int addJumpingPieceMovesto(int sqr, int color, int (*dir)[2], int len, board_t* board, move_t* moves) {
  int mv=0;
  int row=sqr/8, col=sqr%8;
  int oppColor = colorofpiece(color);

  for(int i=0; i<len; i++) {
    int r=row+dir[i][0], c=col+dir[i][1];
    int index = r*8 + c;
    int targetcolor = colorofpiece(board->square[index]);
    if( 0<=r && r<8 && 0<=c && c<8 && targetcolor!=color )
    {
      addMovebyPosition(sqr, index, board->square[index]>0 ? index : -1, board,  moves+mv);
      mv++;
    }
  }

  return mv;
}

int addPawnMovesto(int sqr, int dir, int homerow, int color, int oppColor, board_t* board, move_t* moves) { // dir = 1 for white and -1 for black
  int mv=0;
  int row=sqr/8, col=sqr%8;

   int r,c;
  // 1 step forward
  r=row+dir, c=col;
  if(board->square[r*8+c]==0) {
    addMovebyPosition(sqr, r*8+c, -1, board, moves+mv);
    if(r==7 || r==0) moves[mv].promotion = true;
    mv++;
  }

  // 2 step forward
  r=row+2*dir, c=col;
  if(row==homerow && board->square[r*8+c]==0) {
    addMovebyPosition(sqr, r*8+c, -1, board, moves+mv);
    moves[mv].is_en_passant_pawn = true;
    mv++;
  }

  // captures
  r=row+dir, c=col+1;
  if(0<=c && c<8 && colorofpiece(board->square[r*8+c]) == oppColor) {
    addMovebyPosition(sqr, r*8+c, r*8+c, board, moves+mv);
    if(r==7 || r==0) moves[mv].promotion = true;
    mv++;
  }
  c=col-1;
  if(0<=c && c<8 && colorofpiece(board->square[r*8+c]) == oppColor) {
    addMovebyPosition(sqr, r*8+c, r*8+c, board, moves+mv);
    if(r==7 || r==0) moves[mv].promotion = true;
    mv++;
  }

  // en_passant
  r=row+dir, c=col+1;
  if(0<=c && c<8 && board->en_passant_pawn == row*8+c) {
    addMovebyPosition(sqr, r*8+c, row*8+c, board, moves+mv);
    mv++;
  }
  c=col-1;
  if(0<=c && c<8 && board->en_passant_pawn == row*8+c) {
    addMovebyPosition(sqr, r*8+c, row*8+c, board, moves+mv);
    mv++;
  }

  return mv;
}

// Sliding Pieces
int addRookMovesto(int sqr, int color, board_t* board, move_t* moves) {
  int dirOffset[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
  return addSlidingPieceMovesto(sqr, color, dirOffset, 4, board, moves);
}

int addBishopMovesto(int sqr, int color, board_t* board, move_t* moves) {
  int dirOffset[4][2] = {{-1,-1}, {-1,1}, {1,1}, {1,-1}};
  return addSlidingPieceMovesto(sqr, color, dirOffset, 4, board, moves);
}

int addQueenMovesto(int sqr, int color, board_t* board, move_t* moves) {
  int dirOffset[8][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}, {-1,-1}, {-1,1}, {1,1}, {1,-1}};
  return addSlidingPieceMovesto(sqr, color, dirOffset, 8, board, moves);
}

// Jumping Pieces
int addKingMovesto(int sqr, int color, board_t* board, move_t* moves) {
  int dirOffset[8][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}, {-1,-1}, {-1,1}, {1,1}, {1,-1}};
  return addJumpingPieceMovesto(sqr, color, dirOffset, 8, board, moves);
}

int addKnightMovesto(int sqr, int color, board_t* board, move_t* moves) {
  int dirOffset[8][2] = {{2,1}, {-2,1}, {2,-1}, {-2,-1}, {1,2}, {1,-2}, {-1,2}, {-1,-2}};
  return addJumpingPieceMovesto(sqr, color, dirOffset, 8, board, moves);
}

// Pawns
int addWhitePawnMovesto(int sqr, board_t* board, move_t* moves) {
  return addPawnMovesto(sqr, 1, 1, WHITE, BLACK, board, moves);
}

int addBlackPawnMovesto(int sqr, board_t* board, move_t* moves) {
  return addPawnMovesto(sqr, -1, 6, BLACK, WHITE, board, moves);
}

int addCastlingMove(int kingfrom, int kingto, int rooksqr, int identity, int color, board_t* board, move_t* moves) {
  if( board->castle & identity != identity ) return 0;
  int a = kingfrom<rooksqr ? kingfrom : rooksqr;
  int b = kingfrom<rooksqr ? rooksqr : kingfrom;
  for(int i=a+1; i<b; i++)
    if(board->square[i] != NOPIECE) return 0;

  addMovebyPosition(kingfrom, kingto, -1, board, moves);
  moves[0].is_castling = true;
  return 1; // change this to 1
}

void update_legal_moves(board_t* board, move_t* moves) {
  int moveNum=0;
  for(int i=0; i<64; i++) {
    int piece = board->square[i];
    int color = colorofpiece(piece);
    if(color != board->turn) continue;
          // piece & 00000111 => this removes the color attribute 
         if(typeofpiece(piece) == ROOK) moveNum += addRookMovesto(i, color, board, moves+moveNum);
    else if(typeofpiece(piece) == BISHOP) moveNum += addBishopMovesto(i, color, board, moves+moveNum);
    else if(typeofpiece(piece) == QUEEN) moveNum += addQueenMovesto(i, color, board, moves+moveNum);
    else if(typeofpiece(piece) == KING) moveNum += addKingMovesto(i, color, board, moves+moveNum);
    else if(typeofpiece(piece) == KNIGHT) moveNum += addKnightMovesto(i, color, board, moves+moveNum);
    else if(piece == (BLACK | PAWN)) moveNum += addBlackPawnMovesto(i, board, moves+moveNum);
    else if(piece == (WHITE | PAWN)) moveNum += addWhitePawnMovesto(i, board, moves+moveNum);
  }

  // castling needs attention
  switch(board->turn) {
    case WHITE:
      moveNum += addCastlingMove(4 ,6 ,7 ,8,WHITE, board, moves+moveNum);
      moveNum += addCastlingMove(4 ,2 ,0 ,2,WHITE, board, moves+moveNum);
      break;
    case BLACK:
      moveNum += addCastlingMove(60,62,63,1,BLACK, board, moves+moveNum);
      moveNum += addCastlingMove(60,58,56,4,BLACK, board, moves+moveNum);
      break;
  }
  
  while(moveNum<MOVES_ARR_LEN) {
    addMovebyPosition(-1, -1, -1, board, moves + moveNum);
    moveNum++;
  }
}

move_t getLegalMoveby(int from, int to, board_t* board, move_t* moves) {
  for(int i=0; i<MOVES_ARR_LEN; i++) {
    if(from == moves[i].startsqr && to == moves[i].targetsqr)
      return moves[i];
  }
  return moves[MOVES_ARR_LEN-1];
}

int askPromotionPiece() {
  int piece = QUEEN;
  printf("Promoting to?(Q,R,B,N) ");
  char piecesym;
  while(1) {
    scanf("%s", &piecesym);
    piece = piece_sym[piecesym];
    if(piece == NOPIECE || piece == ' ')
      printf("Invalid piece!! Enter again: ");
    else break;
  }
  return typeofpiece(piece);
}

void makeMove(board_t* board, move_t move) {
  if(move.startsqr == -1 || move.targetsqr == -1) return;

  delPiecefrom(move.captures, board);
  shiftPiece(move.startsqr, move.targetsqr, board);

  if(move.is_en_passant_pawn) setEnPassantPawn(move.targetsqr, board);
  else delEnPassantPawn(board);

  if(move.promotion) {
    int piece = askPromotionPiece(board);
    piece = getTurn(board) | piece;
    setPiece(piece, move.targetsqr, board);
  }

  if(move.is_castling) {
    switch(move.targetsqr) {
      case 6 : shiftPiece(7 ,5 , board); delCastleAbility(12, board); break;
      case 2 : shiftPiece(0 ,3 , board); delCastleAbility(12, board); break;
      case 62: shiftPiece(63,61, board); delCastleAbility(3 , board); break;
      case 58: shiftPiece(56,59, board); delCastleAbility(3 , board); break;
    }
  } else {
    switch(move.startsqr) {
      case 0 : delCastleAbility( 8, board); break;
      case 4 : delCastleAbility(12, board); break;
      case 7 : delCastleAbility( 4, board); break;
      case 56: delCastleAbility( 2, board); break;
      case 60: delCastleAbility( 3, board); break;
      case 63: delCastleAbility( 1, board); break;
    }
  }

  switchTurn(board);
}
