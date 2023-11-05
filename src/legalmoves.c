#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "position.h"
#include "miscfunctions.h"
#include "legalmoves.h"

int dirsR[4] = {NORTH, SOUTH, EAST, WEST};
int dirsB[4] = {NORTH_WEST, NORTH_EAST, SOUTH_WEST, SOUTH_EAST};
int dirsQ[8] = {NORTH_WEST, NORTH, NORTH_EAST, WEST, EAST, SOUTH_WEST, SOUTH, SOUTH_EAST};
int dirsN[8] = {KNIGHT1,KNIGHT2,KNIGHT3,KNIGHT4,KNIGHT5,KNIGHT6,KNIGHT7,KNIGHT8};

int preComputedMoves(int sqr, int dir, int index) {
  static bool computed=false;
  static int allMoves[64][16][8];
  if(!computed) {
    computed=true;
    for(int sqrI=0; sqrI<64; sqrI++) {
      int row = sqrI/8;
      int col = sqrI%8;

      int dirOffsets[8][2] = {
        {1,-1}, {1,0}, {1,1},
        {0,-1}, {0,1},
        {-1,-1}, {-1,0}, {-1,1}
      };
      for(int offsetI=0; offsetI<8; offsetI++) {
        int r = row + dirOffsets[offsetI][0];
        int c = col + dirOffsets[offsetI][1];

        int i=0;
        while(0<=r && r<8 && 0<=c && c<8) {
          allMoves[sqrI][offsetI][i++] = r*8 + c;
          r += dirOffsets[offsetI][0];
          c += dirOffsets[offsetI][1];
        }
        while(i<8) allMoves[sqrI][offsetI][i++] = -1;
      }

      int dirOffsetsK[8][2] = {
        {2,-1}, {2,1},
        {1,-2}, {1,2},
        {-1,-2}, {-1,2},
        {-2,-1}, {-2,1}
      };
      for(int offsetI=8; offsetI<16; offsetI++) {
        int r = row + dirOffsets[offsetI][0];
        int c = col + dirOffsets[offsetI][1];

        int i=0;
        if(0<=r && r<8 && 0<=c && c<8) {
          allMoves[sqrI][offsetI][i++] = r*8 + c;
        }
        while(i<8) allMoves[sqrI][offsetI][i++] = -1;
      }
    }
  }
  
  return allMoves[sqr][dir][index];
}

void sqrsCtrlbySlidingPc(bool out_data[64], int sqr, int* dirs, int dirLen, int squares[64] ) {
  for(int dirI=0; dirI<dirLen; dirI++) {
    int dir = dirs[dirI];
    for(int i=0; preComputedMoves(sqr, dir, i) != -1; i++) {
      int targetSqr = preComputedMoves(sqr,dir,i);
      out_data[targetSqr] = true;
      if(squares[targetSqr] != NOPIECE) break;
    }
  }
}

void sqrsCtrlbyJumpingPc(bool out_data[64], int sqr, int* dirs, int dirLen, int squares[64] ) {
  for(int i=0; i<dirLen; i++) {
    int targetSqr = preComputedMoves(sqr, dirs[i], 0);
    if(targetSqr == -1) continue;
    out_data[targetSqr] = true;
  }
}

void sqrsControlledby(bool out_data[64], int color, int squares[64]) {
  int dirsWP[8] = {NORTH_WEST, NORTH_EAST};
  int dirsBP[8] = {SOUTH_WEST, SOUTH_EAST};

  for(int i=0; i<64; i++) out_data[i]=0;
  for(int i=0; i<64; i++) {
    int piece = squares[i];
    if(colorofpiece(piece) != color) continue;
    switch(typeofpiece(piece)) {
      case ROOK  : sqrsCtrlbySlidingPc(out_data, i, dirsR, 4, squares); break;
      case BISHOP: sqrsCtrlbySlidingPc(out_data, i, dirsB, 4, squares); break;
      case QUEEN : sqrsCtrlbySlidingPc(out_data, i, dirsQ, 8, squares); break;
      case KING  : sqrsCtrlbyJumpingPc(out_data, i, dirsQ, 8, squares); break;
      case KNIGHT: sqrsCtrlbyJumpingPc(out_data, i, dirsN, 8, squares); break;
      case PAWN  :
           if(color == BLACK) sqrsCtrlbyJumpingPc(out_data, i, dirsBP, 2, squares);
           else sqrsCtrlbyJumpingPc(out_data, i, dirsWP, 2, squares);
        break;
    }
  }
}

// Pinned Pieces
int pinnedPieces[64][8];
int checkPins[8]; // When not in check all values(i.e squares) are -1
                  // When in check all indices have some values and each piece can only
                  // move to the squares it contains
                  // When in double check all values are 64, means no piece can move
                  // other than the king itself
void update_pinned_pieces(int color, board_t board) {
  // initialisation
  for(int i=0; i<64; i++) {
    for(int j=0; j<8; j++) pinnedPieces[i][j]=-1;
  }
  for(int i=0; i<8; i++) checkPins[i]=-1;

  int* squares = board.squares;
  int kingPos = searchIn(squares, 64, color | KING);
  int row=kingPos/8, col=kingPos%8;

  // Checking orthoganally and/or diagonally
  for(int dirI=0; dirI<8; dirI++) {
    int pins[8];
    int pinnedSqr=-1;
    int dir = dirI<4 ? dirsR[dirI] : dirsB[dirI-4];

    pins[0] = preComputedMoves(kingPos,dir,0);
    for( int i=0; preComputedMoves(kingPos,dir,i)!=-1; i++) {
      pins[i] = preComputedMoves(kingPos,dir,i);
      int currentPiece = squares[pins[i]];
      if(currentPiece==NOPIECE) continue;

      if(colorofpiece(currentPiece)!=color && (
          ( dirI<4 && (typeofpiece(currentPiece)==ROOK || typeofpiece(currentPiece)==QUEEN)) || 
          ( dirI>=4 && (typeofpiece(currentPiece)==BISHOP || typeofpiece(currentPiece)==QUEEN) )
         )
        ) {
        if(pinnedSqr==-1) {
          if(checkPins[0]==-1) for(int j=0; j<=i; j++) checkPins[j] = pins[j];
          else for(int j=0; j<8; j++) checkPins[j] = 64;
        }
        else for(int j=0; j<=i; j++) pinnedPieces[pinnedSqr][j] = pins[j];
        break;
      }
      else if(colorofpiece(currentPiece)==color) {
        if(pinnedSqr == -1) pinnedSqr = pins[i];
        else break;
      }
      else break;
    }
  }

  // Checking Knight
  int dirOffset2[8][2] = {{2,1}, {-2,1}, {2,-1}, {-2,-1}, {1,2}, {1,-2}, {-1,2}, {-1,-2}};
  for(int dir=KNIGHT1; dir<=KNIGHT8; dir++) {
    int targetSqr = preComputedMoves(kingPos,dir,0);
    if(targetSqr==-1) continue;

    if(squares[targetSqr]==NOPIECE ||
        colorofpiece(squares[targetSqr])==color ||
        typeofpiece(squares[targetSqr])!=KNIGHT) continue;

    if(checkPins[0]==-1) for(int j=0; j<8; j++) checkPins[j]=targetSqr;
    else for(int j=0; j<8; j++) checkPins[j]=64;
  }

  // Checking Pawn
  int forward = color==WHITE ? 1 : -1;
  int dirOffset3[2][2] = {{forward,-1}, {forward,1}};
  for(int dir=0; dir<2; dir++) {
    int r=row+dirOffset3[dir][0],c=col+dirOffset3[dir][1];
    if(!(0<=r && r<8 && 0<=c && c<8)) continue;
    int index = r*8 + c;

    if(squares[index]==NOPIECE ||
        colorofpiece(squares[index])==color ||
        typeofpiece(squares[index])!=PAWN) continue;

    if(checkPins[0]==-1) for(int j=0; j<8; j++) checkPins[j]=index;
    else for(int j=0; j<8; j++) checkPins[j]=64;
  }
}

//
void addMovebyPosition(int startsqr, int targetsqr, int captures, move_t* move) {
  move->startsqr = startsqr;
  move->targetsqr = targetsqr;
  move->captures = captures;
  move->is_en_passant_pawn = false;
  move->is_castling = false;
  move->promotingto = NOPIECE;
}

// These all functions return no. of moves added
//
int addSlidingPieceMovesto(
    int sqr, int (*offsets)[2], int offsetsLen, board_t board, move_t* moves ) {
  int mv=0;
  int row=sqr/8, col=sqr%8;
  int piece = board.squares[sqr];
  int color = colorofpiece(piece);
  int oppColor = oppositecolor(color);

  for(int i=0; i<offsetsLen; i++) {
    for( int r=row+offsets[i][0], c=col+offsets[i][1];
        0<=r && r<8 && 0<=c && c<8;
        r+=offsets[i][0], c+=offsets[i][1] )
    {
      int index = r*8 + c;
      int targetcolor = colorofpiece(board.squares[index]);
      if(color == targetcolor) break;
      if(( pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,index)!=-1 ) &&
          ( checkPins[0]==-1 || searchIn(checkPins,8,index)!=-1 )) {
        addMovebyPosition(sqr, index, board.squares[index]>0 ? index : -1, moves + mv);
        mv++;
      }
      if(oppColor == targetcolor) break;
    }
  }

  return mv;
}

int addSlidingPieceMovestox(int sqr, int* dirs, int dirLen, board_t board, move_t* moves) {
  int mv=0;
  int piece = board.squares[sqr];
  int color = colorofpiece(piece);
  int oppColor = oppositecolor(color);

  for(int dirI=0; dirI<dirLen; dirI++) {
    int dir = dirs[dirI];
    for(int sqrI=0; preComputedMoves(sqr, dir, sqrI)!=-1 && sqrI<8; sqrI++) {
      int targetSqr = preComputedMoves(sqr,dir,sqrI);
      int targetcolor = colorofpiece(board.squares[targetSqr]);
      if(color == targetcolor) break;
      if(( pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,targetSqr)!=-1 ) &&
          ( checkPins[0]==-1 || searchIn(checkPins,8,targetSqr)!=-1 )) {
        addMovebyPosition(sqr, targetSqr, board.squares[targetSqr]>0 ? targetSqr : -1, moves + mv);
        mv++;
      }
      if(oppColor == targetcolor) break;
    }
  }

  return mv;
}

int addJumpingPieceMovesto(int sqr, int* dirs, int dirLen, board_t board, move_t* moves) {
  int mv=0;
  int movingPiece = board.squares[sqr];
  int color = colorofpiece(movingPiece);
  int oppColor = oppositecolor(color);

  bool notAllowedin[64];
  if(typeofpiece(movingPiece) == KING) {
    board.squares[sqr] = NOPIECE;
    sqrsControlledby(notAllowedin, oppColor, board.squares);
    board.squares[sqr] = movingPiece;
  }

  for(int dirI=0; dirI<dirLen; dirI++) {
    int dir = dirs[dirI];
    int targetSqr = preComputedMoves(sqr, dir, 0);
    int targetcolor = colorofpiece(board.squares[targetSqr]);
    if( targetSqr!=-1 && targetcolor!=color &&
        (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,targetSqr)!=-1)) {
      if(typeofpiece(movingPiece)!=KING &&
          !( checkPins[0]==-1 || searchIn(checkPins,8,targetSqr)!=-1 )) continue;
      if(typeofpiece(movingPiece)==KING && notAllowedin[targetSqr]) continue;
      addMovebyPosition(sqr, targetSqr, board.squares[targetSqr]>0 ? targetSqr : -1, moves+mv);
      mv++;
    }
  }

  return mv;
}

int addPawnMovesto(int sqr, board_t board, move_t* moves ){
  int piece = board.squares[sqr];
  int color = colorofpiece(piece);
  int oppColor = oppositecolor(color);
  int homerow = 1;

  int f1 = preComputedMoves(sqr,NORTH,0); // 1*forward
  int f2 = preComputedMoves(sqr,NORTH,1); // 2*forward
  int fl = preComputedMoves(sqr,NORTH_WEST,0); // forward + left
  int fr = preComputedMoves(sqr,NORTH_EAST,0); // forward + right
  int lt = preComputedMoves(sqr,WEST,0); // left
  int rt = preComputedMoves(sqr,EAST,0); // right
  if(color == BLACK) {
    f1 = preComputedMoves(sqr,SOUTH,0);
    f2 = preComputedMoves(sqr,SOUTH,1);
    fl = preComputedMoves(sqr,SOUTH_WEST,0);
    fr = preComputedMoves(sqr,SOUTH_EAST,0);
    homerow = 6;
  }

  int mv=0;
  int row=sqr/8;

  // 1 step forward
  if(board.squares[f1]==NOPIECE &&
      (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,f1)!=-1) &&
      ( checkPins[0]==-1 || searchIn(checkPins,8,f1)!=-1 )) {
    if(f2==-1) {
      int possible_promotions[] = {QUEEN, ROOK, KNIGHT, BISHOP};
      for(int i=0; i<4; i++) {
        addMovebyPosition(sqr, f1, -1, moves+mv);
        moves[mv].promotingto = color | possible_promotions[i];
        mv++;
      }
    }
    else {
      addMovebyPosition(sqr, f1, -1, moves+mv);
      mv++;
    }
  }

  // 2 step forward
  if(row==homerow &&
      board.squares[f2]==NOPIECE && board.squares[f1] == NOPIECE &&
      ( pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,f2)!=-1 ) &&
      ( checkPins[0]==-1 || searchIn(checkPins,8,f2)!=-1 )) {
    addMovebyPosition(sqr, f2, -1, moves+mv);
    if( (lt!=-1 && board.squares[f2-1]==(oppColor|PAWN)) ||
        (rt!=-1  && board.squares[f2+1]==(oppColor|PAWN)) )
      moves[mv].is_en_passant_pawn = true;
    mv++;
  }

  // captures
  if(rt!=-1 && colorofpiece(board.squares[fr]) == oppColor && 
      (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,fr)!=-1) &&
      ( checkPins[0]==-1 || searchIn(checkPins,8,fr)!=-1 )) {
    if(f2==-1) {
      int possible_promotions[] = {QUEEN, ROOK, KNIGHT, BISHOP};
      for(int i=0; i<4; i++) {
        addMovebyPosition(sqr, fr, fr, moves+mv);
        moves[mv].promotingto = color | possible_promotions[i];
        mv++;
      }
    }
    else {
      addMovebyPosition(sqr, fr, fr, moves+mv);
      mv++;
    }
  }
  if(lt!=-1 && colorofpiece(board.squares[fl]) == oppColor && 
      (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,fl)!=-1) &&
      ( checkPins[0]==-1 || searchIn(checkPins,8,fl)!=-1 )) {
    if(f2==-1) {
      int possible_promotions[] = {QUEEN, ROOK, KNIGHT, BISHOP};
      for(int i=0; i<4; i++) {
        addMovebyPosition(sqr, fl, fl, moves+mv);
        moves[mv].promotingto = color | possible_promotions[i];
        mv++;
      }
    }
    else {
      addMovebyPosition(sqr, fl, fl, moves+mv);
      mv++;
    }
  }

  // en_passant
  if(fr!=-1 && board.en_passant_pawn == rt && 
      (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,fr)!=-1) &&
      ( checkPins[0]==-1 || searchIn(checkPins,8,fr)!=-1 || searchIn(checkPins,8,rt)!=-1 ))
  {
    int leftPiece=NOPIECE, rightPiece=NOPIECE;
    for(int i=0; preComputedMoves(sqr,WEST,i)!=-1 && leftPiece==NOPIECE; i++)
      leftPiece = board.squares[preComputedMoves(sqr,WEST,i)];
    for(int i=1; preComputedMoves(sqr,EAST,i)!=-1 && rightPiece==NOPIECE; i++)
      rightPiece = board.squares[preComputedMoves(sqr,EAST,i)];

    if(leftPiece==(color|KING) && (rightPiece==(oppColor|ROOK) || rightPiece==(oppColor|QUEEN))) goto skippedfirst;
    if(rightPiece==(color|KING) && (leftPiece==(oppColor|ROOK) || leftPiece==(oppColor|QUEEN))) goto skippedfirst;

    addMovebyPosition(sqr, fr, sqr+1, moves+mv);
    mv++;
  }
skippedfirst:
  if(fl!=-1 && board.en_passant_pawn == lt && 
      (pinnedPieces[sqr][0]==-1 || searchIn(pinnedPieces[sqr],8,fl)!=-1) &&
      ( checkPins[0]==-1 || searchIn(checkPins,8,fl)!=-1 || searchIn(checkPins,8,lt)!=-1 ))
  {
    int leftPiece=NOPIECE, rightPiece=NOPIECE;
    for(int i=1; preComputedMoves(sqr,WEST,i)!=-1 && leftPiece==NOPIECE; i++)
      leftPiece = board.squares[preComputedMoves(sqr,WEST,i)];
    for(int i=0; preComputedMoves(sqr,EAST,i)!=-1 && rightPiece==NOPIECE; i++)
      rightPiece = board.squares[preComputedMoves(sqr,EAST,i)];

    if(leftPiece==(color|KING) && (rightPiece==(oppColor|ROOK) || rightPiece==(oppColor|QUEEN))) goto skippedsecond;
    if(rightPiece==(color|KING) && (leftPiece==(oppColor|ROOK) || leftPiece==(oppColor|QUEEN))) goto skippedsecond;

    addMovebyPosition(sqr, fl, sqr-1, moves+mv);
    mv++;
  }
skippedsecond:

  return mv;
}

int addCastlingMove(
    int kingfrom, int kingto, int rooksqr, int identity, int color,
    board_t board, move_t* moves ) {
  if( (board.castle & identity) != identity ) return 0;
  if( board.squares[kingfrom]!=(color|KING) || board.squares[rooksqr]!=(color|ROOK) ) return 0;

  bool notAllowedin[64];
  board.squares[kingfrom] = NOPIECE;
  sqrsControlledby(notAllowedin, oppositecolor(color), board.squares);
  board.squares[kingfrom] = color | KING;

  if(notAllowedin[kingfrom]) return 0;
  if(kingfrom<rooksqr) {
    for(int i=kingfrom+1; i<rooksqr; i++)
      if(board.squares[i] != NOPIECE) return 0;
    if(notAllowedin[kingfrom+1] || notAllowedin[kingfrom+2]) return 0;
  }
  else {
    for(int i=kingfrom-1; i>rooksqr; i--)
      if(board.squares[i] != NOPIECE) return 0;
    if(notAllowedin[kingfrom-1] || notAllowedin[kingfrom-2]) return 0;
  }

  addMovebyPosition(kingfrom, kingto, -1, moves);
  moves[0].is_castling = true;
  return 1;
}

int update_legal_moves(board_t board, move_t* moves) {
  // ---Return Values---
  // > 0 : no. of available legal moves
  // = 0 : Checkmate
  // =-1 : Stalemate
  // =-2 : Draw by 50 move rule
  //

  // Checking for Draws
  if(board.halfmove >= 100) return -2;

  update_pinned_pieces(board.turn, board);

  int kingSqr=-1;
  int moveNum=0;
  for(int i=0; i<64; i++) {
    int piece = board.squares[i];
    int color = colorofpiece(piece);
    if(color != board.turn) continue;
    switch(typeofpiece(piece)) {
      case ROOK  : moveNum += addSlidingPieceMovestox(i, dirsR, 4, board, moves+moveNum); break;
      case BISHOP: moveNum += addSlidingPieceMovestox(i, dirsB, 4, board, moves+moveNum); break;
      case QUEEN : moveNum += addSlidingPieceMovestox(i, dirsQ, 8, board, moves+moveNum); break;
      case KING  : moveNum += addJumpingPieceMovesto(i, dirsQ, 8, board, moves+moveNum);
                   kingSqr = i;
                   break;
      case KNIGHT: moveNum += addJumpingPieceMovesto(i, dirsN, 8, board, moves+moveNum); break;
      case PAWN  : moveNum += addPawnMovesto  (i, board, moves+moveNum); break;
    }
  }

  // castling
  switch(board.turn) {
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
    addMovebyPosition(-1, -1, -1, moves + moveNum);
    moveNum++;
  }

  if(moveNumx==0) {
    bool enemy_terittory[64];
    sqrsControlledby(enemy_terittory, oppositecolor(board.turn), board.squares);
    if(enemy_terittory[kingSqr]) return 0;
    else return -1;
  }

  return moveNumx;
}

move_t getLegalMoveby(int from, int to, move_t* moves) {
  for(int i=0; i<MOVES_ARR_LEN; i++) {
    if(from == moves[i].startsqr && to == moves[i].targetsqr)
      return moves[i];
  }
  return moves[MOVES_ARR_LEN-1];
}

board_t makeMove(board_t* boardx, move_t move) {
  board_t board = *boardx; // I know, I know, This looks weird but
                           // Idk why this is causing an error when
                           // I just directly try to use "board_t baord"
                           // parameter instead of "board_t* boardx"
  if(move.startsqr == -1 || move.targetsqr == -1) return board;

  if(typeofpiece(board.squares[move.startsqr]) != PAWN && move.captures == -1)
    board.halfmove++;
  else board.halfmove=0;
  if(board.turn==BLACK) board.fullmove++;

  board.squares[move.captures] = NOPIECE;
  shiftPiece(move.startsqr, move.targetsqr, &board);

  if(move.is_en_passant_pawn) setEnPassantPawn(move.targetsqr, &board);
  else delEnPassantPawn(&board);

  if(move.promotingto!=NOPIECE) {
    int piece = move.promotingto;
    piece = board.turn | piece;
    board.squares[move.targetsqr] = piece;
  }

  if(move.is_castling) {
    switch(move.targetsqr) {
      case 6 :
        shiftPiece(7 ,5 , &board);
        delCastleAbility(WHITE_KING_SIDE | WHITE_QUEEN_SIDE, &board);
        break;
      case 2 :
        shiftPiece(0 ,3 , &board);
        delCastleAbility(WHITE_KING_SIDE | WHITE_QUEEN_SIDE, &board);
        break;
      case 62:
        shiftPiece(63,61, &board);
        delCastleAbility(BLACK_KING_SIDE | BLACK_QUEEN_SIDE, &board);
        break;
      case 58:
        shiftPiece(56,59, &board);
        delCastleAbility(BLACK_KING_SIDE | BLACK_QUEEN_SIDE, &board);
        break;
    }
  } else {
    validate_castle(&board);
  }

  switchTurn(&board);
  return board;
}
