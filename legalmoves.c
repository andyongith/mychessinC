#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "position.h"
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
}

// These all functions return no. of moves added
//
int addSlidingPieceMovesto(int sqr, Move* moves, int color, int (*dir)[2], int len) {
  int mv=0;
  int row=sqr/8, col=sqr%8;
  int oppColor = color ^ (WHITE | BLACK);

  for(int i=0; i<len; i++) {
    for( int r=row+dir[i][0], c=col+dir[i][1];
        0<=r && r<8 && 0<=c && c<8;
        r+=dir[i][0], c+=dir[i][1] )
    {
      int index = r*8 + c;
      int targetcolor = Board.square[index] & (WHITE | BLACK);
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
  int oppColor = color ^ (WHITE | BLACK);

  for(int i=0; i<len; i++) {
    int r=row+dir[i][0], c=col+dir[i][1];
    int index = r*8 + c;
    int targetcolor = Board.square[index] & (WHITE | BLACK);
    if( 0<=r && r<8 && 0<=c && c<8 && targetcolor!=color )
    {
      addMovebyPosition(sqr, index, Board.square[index]>0 ? index : -1, moves+mv);
      mv++;
    }
  }

  return mv;
}

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

int addKingMovesto(int sqr, Move* moves, int color) {
  int dirOffset[8][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}, {-1,-1}, {-1,1}, {1,1}, {1,-1}};
  return addJumpingPieceMovesto(sqr, moves, color, dirOffset, 8);
}

int addKnightMovesto(int sqr, Move* moves, int color) {
  int dirOffset[8][2] = {{2,1}, {-2,1}, {2,-1}, {-2,-1}, {1,2}, {1,-2}, {-1,2}, {-1,-2}};
  return addJumpingPieceMovesto(sqr, moves, color, dirOffset, 8);
}

int addPawnMovesto(int sqr, Move* moves, int dir, int homerow, int color, int oppColor) { // dir = 1 for white and -1 for black
  int mv=0;
  int row=sqr/8, col=sqr%8;

   int r,c;
  // 1 step forward
  r=row+dir, c=col;
  if(Board.square[r*8+c]==0)
    addMovebyPosition(sqr, r*8+c, -1, moves+(mv++));

  // 2 step forward
  r=row+2*dir, c=col;
  if(row==homerow && Board.square[r*8+c]==0) {
    addMovebyPosition(sqr, r*8+c, -1, moves+mv);
    // en_passant_pawn = r*8+c; // This should be placed in makeMove function
    moves[mv].is_en_passant_pawn = true;
    mv++;
  }

  // captures
  r=row+dir, c=col+1;
  if(0<=c && c<8 && (Board.square[r*8+c] & oppColor) == oppColor)
    addMovebyPosition(sqr, r*8+c, r*8+c, moves+(mv++));
  c=col-1;
  if(0<=c && c<8 && (Board.square[r*8+c] & oppColor) == oppColor)
    addMovebyPosition(sqr, r*8+c, r*8+c, moves+(mv++));

  // en_passant
  r=row+dir, c=col+1;
  if(0<=c && c<8 && Board.en_passant_pawn == row*8+c) {
    printf("\nEN_PASANT AVAILABLE!\n");
    addMovebyPosition(sqr, r*8+c, row*8+c, moves+mv);
    mv++;
  }
  c=col-1;
  if(0<=c && c<8 && Board.en_passant_pawn == row*8+c) {
    printf("\nEN_PASANT AVAILABLE!\n");
    addMovebyPosition(sqr, r*8+c, row*8+c, moves+mv);
    mv++;
  }

  // promotion

  return mv;
}

int addWhitePawnMovesto(int sqr, Move* moves) {
  return addPawnMovesto(sqr, moves, 1, 1, WHITE, BLACK);
}

int addBlackPawnMovesto(int sqr, Move* moves) {
  return addPawnMovesto(sqr, moves, -1, 6, BLACK, WHITE);
}

void calculate_moves() {
  int moveNum=0;
  for(int i=0; i<64; i++) {
    int piece = Board.square[i];
    int color = piece & (WHITE | BLACK);
    if(color != Board.turn) continue;
          // piece & 00000111 => this removes the color attribute 
         if((piece&7) == ROOK) moveNum += addRookMovesto(i, valid_moves+moveNum, color);
    else if((piece&7) == BISHOP) moveNum += addBishopMovesto(i, valid_moves+moveNum, color);
    else if((piece&7) == QUEEN) moveNum += addQueenMovesto(i, valid_moves+moveNum, color);
    else if((piece&7) == KING) moveNum += addKingMovesto(i, valid_moves+moveNum, color);
    else if((piece&7) == KNIGHT) moveNum += addKnightMovesto(i, valid_moves+moveNum, color);
    else if(piece == (BLACK | PAWN)) moveNum += addBlackPawnMovesto(i, valid_moves+moveNum);
    else if(piece == (WHITE | PAWN)) moveNum += addWhitePawnMovesto(i, valid_moves+moveNum);
  }

  // castling needs attention
  
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

void makeMove(Move move) {
  if(move.startsqr == -1 || move.targetsqr == -1) return;
  removeSquare(move.captures);
  shiftSquare(move.startsqr, move.targetsqr);
  if(move.is_en_passant_pawn) updateEnPassantPawn(move.targetsqr);
  else removeEnPassantPawn();
  changeTurn();
}

void printLegalMoves() {
  printf("----Legal Moves----\n");

  for(int i=0; i<MOVES_ARR_LEN && valid_moves[i].startsqr!=-1; i++) {
    char sqr1[] = "a1", sqr2[] = "a1", sqr3[] = "a1";
    numToName(valid_moves[i].startsqr, sqr1);
    numToName(valid_moves[i].targetsqr, sqr2);
    numToName(valid_moves[i].captures, sqr3);

    printf("(%s,%s,{%s})\t", sqr1, sqr2, sqr3);
    // printf("%s ", sqr2);
  }
  printf("\n");
}
