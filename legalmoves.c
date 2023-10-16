#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "position.h"
#include "miscfunctions.h"
#include "legalmoves.h"

void sqrsCtrlbySlidingPc(
    bool out_data[64], int index, int (*dir)[2], int len, int in_squares[64] ) {
  int row=index/8, col=index%8;

  for(int i=0; i<len; i++) {
    for( int r=row+dir[i][0], c=col+dir[i][1];
        0<=r && r<8 && 0<=c && c<8;
        r+=dir[i][0], c+=dir[i][1] )
    {
      int targetindex = r*8 + c;
      out_data[targetindex] = true;
      if(in_squares[targetindex] != NOPIECE) break;
    }
  }
}

void sqrsCtrlbyJumpingPc(
    bool out_data[64], int index, int (*dir)[2], int len, int in_squares[64] ) {
  int row=index/8, col=index%8;

  for(int i=0; i<len; i++) {
    int r=row+dir[i][0], c=col+dir[i][1];
    int targetindex = r*8 + c;
    if(!( 0<=r && r<8 && 0<=c && c<8 )) continue;
    out_data[targetindex] = true;
  }
}

// Sliding Pieces
void sqrsControlledbyRook(bool out_data[64], int index, int in_squares[64]) {
  int dirOffset[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
  sqrsCtrlbySlidingPc(out_data, index, dirOffset, 4, in_squares);
}
void sqrsControlledbyBishop(bool out_data[64], int index, int in_squares[64]) {
  int dirOffset[4][2] = {{-1,-1}, {-1,1}, {1,1}, {1,-1}};
  sqrsCtrlbySlidingPc(out_data, index, dirOffset, 4, in_squares);
}
void sqrsControlledbyQueen(bool out_data[64], int index, int in_squares[64]) {
  int dirOffset[8][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}, {-1,-1}, {-1,1}, {1,1}, {1,-1}};
  sqrsCtrlbySlidingPc(out_data, index, dirOffset, 8, in_squares);
}

// Jumping Pieces
void sqrsControlledbyKing(bool out_data[64], int index, int in_squares[64]) {
  int dirOffset[8][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}, {-1,-1}, {-1,1}, {1,1}, {1,-1}};
  sqrsCtrlbyJumpingPc(out_data, index, dirOffset, 8, in_squares);
}
void sqrsControlledbyKnight(bool out_data[64], int index, int in_squares[64]) {
  int dirOffset[8][2] = {{2,1}, {-2,1}, {2,-1}, {-2,-1}, {1,2}, {1,-2}, {-1,2}, {-1,-2}};
  sqrsCtrlbyJumpingPc(out_data, index, dirOffset, 8, in_squares);
}

// Pawns
void sqrsControlledbyWhitePawn(bool out_data[64], int index, int in_squares[64]) {
  int dirOffset[2][2] = {{1,-1}, {1,1}};
  sqrsCtrlbyJumpingPc(out_data, index, dirOffset, 2, in_squares);
}
void sqrsControlledbyBlackPawn(bool out_data[64], int index, int in_squares[64]) {
  int dirOffset[2][2] = {{-1,-1}, {-1,1}};
  sqrsCtrlbyJumpingPc(out_data, index, dirOffset, 2, in_squares);
}

void sqrsControlledby(bool out_data[64], int color, int in_squares[64]) {
  for(int i=0; i<64; i++) out_data[i]=0;
  for(int i=0; i<64; i++) {
    int piece = in_squares[i];
    if(colorofpiece(piece) != color) continue;
    switch(typeofpiece(piece)) {
      case ROOK  : sqrsControlledbyRook  (out_data, i, in_squares); break;
      case BISHOP: sqrsControlledbyBishop(out_data, i, in_squares); break;
      case QUEEN : sqrsControlledbyQueen (out_data, i, in_squares); break;
      case KING  : sqrsControlledbyKing  (out_data, i, in_squares); break;
      case KNIGHT: sqrsControlledbyKnight(out_data, i, in_squares); break;
      case PAWN  :
             if(color == BLACK) sqrsControlledbyBlackPawn(out_data, i, in_squares);
        else if(color == WHITE) sqrsControlledbyWhitePawn(out_data, i, in_squares);
        break;
    }
  }
}

// Pinned Pieces
int pinnedPieces[64][8];
int checkPines[8]; // When not in check all values(i.e squares) are -1
                   // When in check all indices have some values and each piece can only
                   // move to the squares it contains
                   // When in check all values are 64, means no piece can move other than
                   // the king itself
void update_pinned_pieces(int color, board_t* board) {
  for(int i=0; i<64; i++) {
    for(int j=0; j<8; j++) pinnedPieces[i][j]=-1;
  }
  for(int i=0; i<8; i++) checkPines[i]=-1;
  int* squares = board->square;
  int kingPos = searchIn(squares, 64, color | KING);
  int row=kingPos/8, col=kingPos%8;

  int dirOffset[8][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}, {-1,-1}, {-1,1}, {1,1}, {1,-1}};
  for(int dir=0; dir<8; dir++) {
    int pines[8];
    int pinSqr=-1;

    for( int r=row+dirOffset[dir][0],c=col+dirOffset[dir][1], i=0;
        0<=r && r<8 && 0<=c && c<8;
        r+=dirOffset[dir][0], c+=dirOffset[dir][1], i++ ) {
      pines[i] = r*8 + c;
      if(squares[pines[i]]==NOPIECE) continue;

      int currentPiece = squares[pines[i]];
      if(colorofpiece(currentPiece)!=color) {
        if(pinSqr==-1) {
          switch(checkPines[0]) {
            case -1: for(int j=0; j<=i; j++) checkPines[j]=pines[j]; break;
            case 64: break;
            default: for(int j=0; j<8; j++) checkPines[j]=64;
          }
          break;
        }
        if(dir<4 && !(typeofpiece(currentPiece)==ROOK || typeofpiece(currentPiece)==QUEEN)) break;
        if(dir>=4 && !(typeofpiece(currentPiece)==BISHOP || typeofpiece(currentPiece)==QUEEN)) break;
        else {
          int j=0;
          while(j<=i) {
            pinnedPieces[pinSqr][j] = pines[j];
            j++;
          }
          while(j<=0) pinnedPieces[pinSqr][j++] = -1;
          break;
        }
      }
      if(pinSqr==-1) {
        pinSqr = pines[i];
        continue;
      }
    }
  }

  int dirOffset2[8][2] = {{2,1}, {-2,1}, {2,-1}, {-2,-1}, {1,2}, {1,-2}, {-1,2}, {-1,-2}};
  for(int dir=0; dir<8; dir++) {
    int r=row+dirOffset[dir][0],c=col+dirOffset[dir][1];
    if(!(0<=r && r<8 && 0<=c && c<8)) continue;
    int index = r*8 + c;
    if(squares[index]==NOPIECE ||
        colorofpiece(squares[index])==color || typeofpiece(squares[index])!=KNIGHT) continue;

    switch(checkPines[0]) {
      case -1: checkPines[0]=index; break;
      case 64: break;
      default: for(int j=0; j<8; j++) checkPines[j]=64;
    }
  }
}

//
void addMovebyPosition(
    int startsqr, int targetsqr, int captures, board_t* board, move_t* move ) {
  move->startsqr = startsqr;
  move->targetsqr = targetsqr;
  move->captures = captures;
  move->is_en_passant_pawn = false;
  move->is_castling = false;
  move->promotion = false;
}

// These all functions return no. of moves added
//
int addSlidingPieceMovesto(
    int sqr, int color, int (*dir)[2], int len, board_t* board, move_t* moves ) {
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
      if(( pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,index)!=-1 ) &&
          ( checkPines[0]==-1 || searchIn(checkPines,8,index)!=-1 )) {
        addMovebyPosition(sqr, index, board->square[index]>0 ? index : -1, board, moves + mv);
        mv++;
      }
      if(oppColor == targetcolor) break;
    }
  }

  return mv;
}

int addJumpingPieceMovesto(
    int sqr, int color, int (*dir)[2], int len, board_t* board, move_t* moves ) {
  int mv=0;
  int row=sqr/8, col=sqr%8;
  int oppColor = oppositecolor(color);
  int movingPiece = board->square[sqr];

  bool notAllowedin[64];
  if(typeofpiece(movingPiece) == KING) {
    delPiecefrom(sqr, board);
    sqrsControlledby(notAllowedin, oppColor, board->square);
    setPieceto(movingPiece, sqr, board);
  }

  for(int i=0; i<len; i++) {
    int r=row+dir[i][0], c=col+dir[i][1];
    int index = r*8 + c;
    int targetcolor = colorofpiece(board->square[index]);
    if( 0<=r && r<8 && 0<=c && c<8 && targetcolor!=color &&
        (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,index)!=-1)) {
      if(typeofpiece(movingPiece)!=KING &&
          !( checkPines[0]==-1 || searchIn(checkPines,8,r*8+c)!=-1 )) continue;
      if(typeofpiece(movingPiece)==KING && notAllowedin[index]) continue;
      addMovebyPosition(sqr, index, board->square[index]>0 ? index : -1, board,  moves+mv);
      mv++;
    }
  }

  return mv;
}

int addPawnMovesto(
    int sqr, int dir, int homerow, int color, int oppColor, board_t* board, move_t* moves ){
  // dir = 1 for white and -1 for black
  int mv=0;
  int row=sqr/8, col=sqr%8;

   int r,c;
  // 1 step forward
  r=row+dir, c=col;
  if(board->square[r*8+c]==0 &&
      (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,r*8+c)!=-1) &&
      ( checkPines[0]==-1 || searchIn(checkPines,8,r*8+c)!=-1 )) {
    addMovebyPosition(sqr, r*8+c, -1, board, moves+mv);
    if(r==7 || r==0) moves[mv].promotion = true;
    mv++;
  }

  // 2 step forward
  r=row+2*dir, c=col;
  if(row==homerow && board->square[r*8+c]==0 && 
      (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,r*8+c)!=-1) &&
      ( checkPines[0]==-1 || searchIn(checkPines,8,r*8+c)!=-1 )) {
    addMovebyPosition(sqr, r*8+c, -1, board, moves+mv);
    moves[mv].is_en_passant_pawn = true;
    mv++;
  }

  // captures
  r=row+dir, c=col+1;
  if(0<=c && c<8 && colorofpiece(board->square[r*8+c]) == oppColor && 
      (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,r*8+c)!=-1) &&
      ( checkPines[0]==-1 || searchIn(checkPines,8,r*8+c)!=-1 )) {
    addMovebyPosition(sqr, r*8+c, r*8+c, board, moves+mv);
    if(r==7 || r==0) moves[mv].promotion = true;
    mv++;
  }
  c=col-1;
  if(0<=c && c<8 && colorofpiece(board->square[r*8+c]) == oppColor && 
      (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,r*8+c)!=-1) &&
      ( checkPines[0]==-1 || searchIn(checkPines,8,r*8+c)!=-1 )) {
    addMovebyPosition(sqr, r*8+c, r*8+c, board, moves+mv);
    if(r==7 || r==0) moves[mv].promotion = true;
    mv++;
  }

  // en_passant
  r=row+dir, c=col+1;
  if(0<=c && c<8 && board->en_passant_pawn == row*8+c && 
      (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,r*8+c)!=-1) &&
      ( checkPines[0]==-1 || searchIn(checkPines,8,r*8+c)!=-1 ))
  {
    int leftPiece=NOPIECE, rightPiece=NOPIECE;
    for(int i=col-1; 0<=i && leftPiece==NOPIECE; i++)
      leftPiece = board->square[row*8+i];
    for(int i=col+2; i<8 && rightPiece==NOPIECE; i++)
      rightPiece = board->square[row*8+i];

    if(leftPiece==(color|KING) && (rightPiece==(oppColor|ROOK) || rightPiece==(oppColor|QUEEN))) goto skippedfirst;
    if(rightPiece==(color|KING) && (leftPiece==(oppColor|ROOK) || leftPiece==(oppColor|QUEEN))) goto skippedfirst;

    addMovebyPosition(sqr, r*8+c, row*8+c, board, moves+mv);
    mv++;
  }
skippedfirst:
  c=col-1;
  if(0<=c && c<8 && board->en_passant_pawn == row*8+c && 
      (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,r*8+c)!=-1) &&
      ( checkPines[0]==-1 || searchIn(checkPines,8,r*8+c)!=-1 ))
  {
    int leftPiece=NOPIECE, rightPiece=NOPIECE;
    for(int i=col-1; 0<=i && leftPiece==NOPIECE; i++)
      leftPiece = board->square[row*8+i];
    for(int i=col+2; i<8 && rightPiece==NOPIECE; i++)
      rightPiece = board->square[row*8+i];

    if(leftPiece==(color|KING) && (rightPiece==(oppColor|ROOK) || rightPiece==(oppColor|QUEEN))) goto skippedsecond;
    if(rightPiece==(color|KING) && (leftPiece==(oppColor|ROOK) || leftPiece==(oppColor|QUEEN))) goto skippedsecond;

    addMovebyPosition(sqr, r*8+c, row*8+c, board, moves+mv);
    mv++;
  }
skippedsecond:

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

int addCastlingMove(
    int kingfrom, int kingto, int rooksqr, int identity, int color,
    board_t* board, move_t* moves ) {
  if( (board->castle & identity) != identity ) return 0;

  bool notAllowedin[64];
  delPiecefrom(kingfrom, board);
  sqrsControlledby(notAllowedin, oppositecolor(color), board->square);
  setPieceto(color | KING, kingfrom, board);

  if(notAllowedin[kingfrom]) return 0;
  if(kingfrom<rooksqr) {
    for(int i=kingfrom+1; i<rooksqr; i++)
      if(board->square[i] != NOPIECE) return 0;
    if(notAllowedin[kingfrom+1] || notAllowedin[kingfrom+2]) return 0;
  }
  else {
    for(int i=kingfrom-1; i>rooksqr; i--)
      if(board->square[i] != NOPIECE) return 0;
    if(notAllowedin[kingfrom-1] || notAllowedin[kingfrom-2]) return 0;
  }

  addMovebyPosition(kingfrom, kingto, -1, board, moves);
  moves[0].is_castling = true;
  return 1;
}

int update_legal_moves(board_t* board, move_t* moves) {
  update_pinned_pieces(board->turn, board);

  int moveNum=0;
  for(int i=0; i<64; i++) {
    int piece = board->square[i];
    int color = colorofpiece(piece);
    if(color != board->turn) continue;
    switch(typeofpiece(piece)) {
      case ROOK  : moveNum += addRookMovesto  (i, color, board, moves+moveNum); break;
      case BISHOP: moveNum += addBishopMovesto(i, color, board, moves+moveNum); break;
      case QUEEN : moveNum += addQueenMovesto (i, color, board, moves+moveNum); break;
      case KING  : moveNum += addKingMovesto  (i, color, board, moves+moveNum); break;
      case KNIGHT: moveNum += addKnightMovesto(i, color, board, moves+moveNum); break;
      case PAWN  :
             if(color == BLACK) moveNum += addBlackPawnMovesto(i, board, moves+moveNum);
        else if(color == WHITE) moveNum += addWhitePawnMovesto(i, board, moves+moveNum);
        break;
    }
  }

  // castling
  switch(board->turn) {
    case WHITE:
      moveNum += addCastlingMove(4 ,6 ,7 ,WHITE_KING_SIDE ,WHITE, board, moves+moveNum);
      moveNum += addCastlingMove(4 ,2 ,0 ,WHITE_QUEEN_SIDE,WHITE, board, moves+moveNum);
      break;
    case BLACK:
      moveNum += addCastlingMove(60,62,63,BLACK_KING_SIDE ,BLACK, board, moves+moveNum);
      moveNum += addCastlingMove(60,58,56,BLACK_QUEEN_SIDE,BLACK, board, moves+moveNum);
      break;
  }
  
  int moveNumx = moveNum;
  while(moveNum<MOVES_ARR_LEN) {
    addMovebyPosition(-1, -1, -1, board, moves + moveNum);
    moveNum++;
  }

  return moveNumx;
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
    setPieceto(piece, move.targetsqr, board);
  }

  if(move.is_castling) {
    switch(move.targetsqr) {
      case 6 : shiftPiece(7 ,5 , board); delCastleAbility(WHITE_KING_SIDE | WHITE_QUEEN_SIDE, board); break;
      case 2 : shiftPiece(0 ,3 , board); delCastleAbility(WHITE_KING_SIDE | WHITE_QUEEN_SIDE, board); break;
      case 62: shiftPiece(63,61, board); delCastleAbility(BLACK_KING_SIDE | BLACK_QUEEN_SIDE, board); break;
      case 58: shiftPiece(56,59, board); delCastleAbility(BLACK_KING_SIDE | BLACK_QUEEN_SIDE, board); break;
    }
  } else {
    switch(move.startsqr) {
      case 0 : delCastleAbility(WHITE_QUEEN_SIDE, board); break;
      case 4 : delCastleAbility(WHITE_KING_SIDE | WHITE_QUEEN_SIDE, board); break;
      case 7 : delCastleAbility(WHITE_KING_SIDE, board); break;
      case 56: delCastleAbility(BLACK_QUEEN_SIDE, board); break;
      case 60: delCastleAbility(BLACK_KING_SIDE | BLACK_QUEEN_SIDE, board); break;
      case 63: delCastleAbility(BLACK_KING_SIDE, board); break;
    }
    switch(move.targetsqr) {
      case 0 : delCastleAbility(WHITE_QUEEN_SIDE, board); break;
      case 4 : delCastleAbility(WHITE_KING_SIDE , board); break;
      case 56: delCastleAbility(BLACK_QUEEN_SIDE, board); break;
      case 63: delCastleAbility(BLACK_KING_SIDE , board); break;
    }
  }

  switchTurn(board);
}
