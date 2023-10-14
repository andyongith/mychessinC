#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "position.h"
#include "miscfunctions.h"
#include "legalmoves.h"

#define MOVES_ARR_LEN 256

Move valid_moves[MOVES_ARR_LEN];

void numToName(int num, char name[3]) {
  if(num==-1) strcpy(name, "  ");
  else {
    name[0] = num%8 + 'a';
    name[1] = num/8 + '1';
    name[2] = '\0';
  }
}

int nameToNum(char name[3]) {
  if(name[0]<'a' || name[0]>'z' || name[1]<'1' || name[1]>'8') return 0;
  int col = name[0]-'a';
  int row = name[1]-'1';
  return row*8 + col;
}

void addMovebyPosition(int startsqr, int targetsqr, int captures, Move* movePos) {
  movePos->startsqr = startsqr;
  movePos->targetsqr = targetsqr;
  movePos->captures = captures;
  movePos->is_en_passant_pawn = false;
  movePos->is_castling = false;
  movePos->promotion = false;
}

// These all functions return no. of moves added
//
int addSlidingPieceMovesto(int sqr, Move* moves, int color, int (*dir)[2], int len) {
  int mv=0;
  int row=sqr/8, col=sqr%8;
  int oppColor = oppositecolor(color);

  for(int i=0; i<len; i++) {
    for( int r=row+dir[i][0], c=col+dir[i][1];
        0<=r && r<8 && 0<=c && c<8;
        r+=dir[i][0], c+=dir[i][1] )
    {
      int index = r*8 + c;
      int targetcolor = colorofpiece(Board.square[index]);
      if(color == targetcolor) break;
      addMovebyPosition(sqr, index, Board.square[index]>0 ? index : -1, moves + mv);
      mv++;
      if(oppColor == targetcolor) break;
    }
  }

  return mv;
}

int addJumpingPieceMovesto(int sqr, Move* moves, int color, int (*dir)[2], int len) {
  int mv=0;
  int row=sqr/8, col=sqr%8;
  int oppColor = colorofpiece(color);

  for(int i=0; i<len; i++) {
    int r=row+dir[i][0], c=col+dir[i][1];
    int index = r*8 + c;
    int targetcolor = colorofpiece(Board.square[index]);
    if( 0<=r && r<8 && 0<=c && c<8 && targetcolor!=color )
    {
      addMovebyPosition(sqr, index, Board.square[index]>0 ? index : -1, moves+mv);
      mv++;
    }
  }

  return mv;
}

int addPawnMovesto(int sqr, Move* moves, int dir, int homerow, int color, int oppColor) { // dir = 1 for white and -1 for black
  int mv=0;
  int row=sqr/8, col=sqr%8;

   int r,c;
  // 1 step forward
  r=row+dir, c=col;
  if(Board.square[r*8+c]==0) {
    addMovebyPosition(sqr, r*8+c, -1, moves+mv);
    if(r==7 || r==0) moves[mv].promotion = true;
    mv++;
  }

  // 2 step forward
  r=row+2*dir, c=col;
  if(row==homerow && Board.square[r*8+c]==0) {
    addMovebyPosition(sqr, r*8+c, -1, moves+mv);
    moves[mv].is_en_passant_pawn = true;
    mv++;
  }

  // captures
  r=row+dir, c=col+1;
  if(0<=c && c<8 && colorofpiece(Board.square[r*8+c]) == oppColor) {
    addMovebyPosition(sqr, r*8+c, r*8+c, moves+mv);
    if(r==7 || r==0) moves[mv].promotion = true;
    mv++;
  }
  c=col-1;
  if(0<=c && c<8 && colorofpiece(Board.square[r*8+c]) == oppColor) {
    addMovebyPosition(sqr, r*8+c, r*8+c, moves+mv);
    if(r==7 || r==0) moves[mv].promotion = true;
    mv++;
  }

  // en_passant
  r=row+dir, c=col+1;
  if(0<=c && c<8 && Board.en_passant_pawn == row*8+c) {
    addMovebyPosition(sqr, r*8+c, row*8+c, moves+mv);
    mv++;
  }
  c=col-1;
  if(0<=c && c<8 && Board.en_passant_pawn == row*8+c) {
    addMovebyPosition(sqr, r*8+c, row*8+c, moves+mv);
    mv++;
  }

  return mv;
}

// Sliding Pieces
int addRookMovesto(int sqr, Move* moves, int color) {
  int dirOffset[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
  return addSlidingPieceMovesto(sqr, moves, color, dirOffset, 4);
}

int addBishopMovesto(int sqr, Move* moves, int color) {
  int dirOffset[4][2] = {{-1,-1}, {-1,1}, {1,1}, {1,-1}};
  return addSlidingPieceMovesto(sqr, moves, color, dirOffset, 4);
}

int addQueenMovesto(int sqr, Move* moves, int color) {
  int dirOffset[8][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}, {-1,-1}, {-1,1}, {1,1}, {1,-1}};
  return addSlidingPieceMovesto(sqr, moves, color, dirOffset, 8);
}

// Jumping Pieces
int addKingMovesto(int sqr, Move* moves, int color) {
  int dirOffset[8][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}, {-1,-1}, {-1,1}, {1,1}, {1,-1}};
  return addJumpingPieceMovesto(sqr, moves, color, dirOffset, 8);
}

int addKnightMovesto(int sqr, Move* moves, int color) {
  int dirOffset[8][2] = {{2,1}, {-2,1}, {2,-1}, {-2,-1}, {1,2}, {1,-2}, {-1,2}, {-1,-2}};
  return addJumpingPieceMovesto(sqr, moves, color, dirOffset, 8);
}

// Pawns
int addWhitePawnMovesto(int sqr, Move* moves) {
  return addPawnMovesto(sqr, moves, 1, 1, WHITE, BLACK);
}

int addBlackPawnMovesto(int sqr, Move* moves) {
  return addPawnMovesto(sqr, moves, -1, 6, BLACK, WHITE);
}

int addCastlingMove(int kingfrom, int kingto, int rooksqr, int identity, int color, Move* moves) {
  if( Board.castle & identity != identity ) return 0;
  int a = kingfrom<rooksqr ? kingfrom : rooksqr;
  int b = kingfrom<rooksqr ? rooksqr : kingfrom;
  for(int i=a+1; i<b; i++)
    if(Board.square[i] != NOPIECE) return 0;

  addMovebyPosition(kingfrom, kingto, -1, moves);
  moves[0].is_castling = true;
  return 1; // change this to 1
}

void calculate_moves() {
  int moveNum=0;
  for(int i=0; i<64; i++) {
    int piece = Board.square[i];
    int color = colorofpiece(piece);
    if(color != Board.turn) continue;
          // piece & 00000111 => this removes the color attribute 
         if(typeofpiece(piece) == ROOK) moveNum += addRookMovesto(i, valid_moves+moveNum, color);
    else if(typeofpiece(piece) == BISHOP) moveNum += addBishopMovesto(i, valid_moves+moveNum, color);
    else if(typeofpiece(piece) == QUEEN) moveNum += addQueenMovesto(i, valid_moves+moveNum, color);
    else if(typeofpiece(piece) == KING) moveNum += addKingMovesto(i, valid_moves+moveNum, color);
    else if(typeofpiece(piece) == KNIGHT) moveNum += addKnightMovesto(i, valid_moves+moveNum, color);
    else if(piece == (BLACK | PAWN)) moveNum += addBlackPawnMovesto(i, valid_moves+moveNum);
    else if(piece == (WHITE | PAWN)) moveNum += addWhitePawnMovesto(i, valid_moves+moveNum);
  }

  // castling needs attention
  switch(Board.turn) {
    case WHITE:
      moveNum += addCastlingMove(4 ,6 ,7 ,8,WHITE, valid_moves+moveNum);
      moveNum += addCastlingMove(4 ,2 ,0 ,2,WHITE, valid_moves+moveNum);
      break;
    case BLACK:
      moveNum += addCastlingMove(60,62,63,1,BLACK, valid_moves+moveNum);
      moveNum += addCastlingMove(60,58,56,4,BLACK, valid_moves+moveNum);
      break;
  }
  
  while(moveNum<MOVES_ARR_LEN) {
    addMovebyPosition(-1, -1, -1, valid_moves + moveNum);
    moveNum++;
  }
}

Move getMoveby(int from, int to) {
  for(int i=0; i<MOVES_ARR_LEN; i++) {
    if(from == valid_moves[i].startsqr && to == valid_moves[i].targetsqr)
      return valid_moves[i];
  }
  return valid_moves[MOVES_ARR_LEN-1];
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

void makeMove(Move move) {
  if(move.startsqr == -1 || move.targetsqr == -1) return;

  removeSquare(move.captures);
  shiftSquare(move.startsqr, move.targetsqr);

  if(move.is_en_passant_pawn) updateEnPassantPawn(move.targetsqr);
  else removeEnPassantPawn();

  if(move.promotion) {
    int piece = askPromotionPiece();
    piece = currentTurn() | piece;
    putSquare(piece, move.targetsqr);
  }

  if(move.is_castling) {
    switch(move.targetsqr) {
      case 6 : shiftSquare(7 ,5 ); delCastleAbility(12); break;
      case 2 : shiftSquare(0 ,3 ); delCastleAbility(12); break;
      case 62: shiftSquare(63,61); delCastleAbility(3 ); break;
      case 58: shiftSquare(56,59); delCastleAbility(3 ); break;
    }
  } else {
    switch(move.startsqr) {
      case 0 : delCastleAbility( 8); break;
      case 4 : delCastleAbility(12); break;
      case 7 : delCastleAbility( 4); break;
      case 56: delCastleAbility( 2); break;
      case 60: delCastleAbility( 3); break;
      case 63: delCastleAbility( 1); break;
    }
  }

  changeTurn();
}

void printLegalMoves() {
  printf("----Legal Moves----\n");

  for(int i=0; i<MOVES_ARR_LEN && valid_moves[i].startsqr!=-1; i++) {
    char sqr1[] = "z9", sqr2[] = "z9", sqr3[] = "z9";
    numToName(valid_moves[i].startsqr, sqr1);
    numToName(valid_moves[i].targetsqr, sqr2);
    numToName(valid_moves[i].captures, sqr3);

    printf("(%s,%s,{%s}%d)\t", sqr1, sqr2, sqr3, valid_moves[i].is_castling);
    // printf("%s ", sqr2);
  }
  printf("\n");
}
