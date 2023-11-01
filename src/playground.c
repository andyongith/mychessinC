#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "playground.h"
#include "miscfunctions.h"
#include "position.h"
#include "display.h"
#include "legalmoves.h"

void play_randomly(board_t board) {
  move_t valid_moves[MOVES_ARR_LEN];
  int num_of_moves = update_legal_moves(board, valid_moves);

  while(num_of_moves>0) {
    printf("FEN: ");
    printFen(board);
    printf("\n");
    drawBoard(board);
    printf("num_of_available_moves: %d\n", num_of_moves);
    if(showlegals) printLegalMoves(valid_moves);
    
    sleep(botmvdelay);
    char from[3], to[3];
    int moveIndex = rand() % num_of_moves;
    // printf("(%d) ", moveIndex);
    move_t move = valid_moves[moveIndex];
    indexToName(move.startsqr, from);
    indexToName(move.targetsqr, to);
    printf("\nbot played: %s %s\n", from, to);
    board = makeMove(&board, move);

    num_of_moves = update_legal_moves(board, valid_moves);
    // sleep(botmvdelay);
  }
  printf("FEN: ");
  printFen(board);
  printf("\n");
  drawBoard(board);
  
  switch(num_of_moves) {
    case  0: printf("\033[31m\"Checkmate\"\033[30m\n"); break;
    case -1: printf("\033[31m\"Stalemate\"\033[30m\n"); break;
    case -2: printf("\033[31m\"Draw by 50 move rule\"\033[30m\n"); break;
    default: printf("\033[31m\"Something's wrong, and idk what.\"\033[30m\n"); break;
  }
}

void play_manually(int side, board_t board) { // side can be WHITE, BLACK or any other number for both
  move_t valid_moves[MOVES_ARR_LEN];
  int num_of_moves = update_legal_moves(board, valid_moves);

  while(num_of_moves>0) {
    printf("FEN: ");
    printFen(board);
    printf("\n");
    drawBoard(board);
    printf("num_of_available_moves: %d\n", num_of_moves);
    if(showlegals) printLegalMoves(valid_moves);
    
    char from[3], to[3];

    if(( side==WHITE && board.turn==BLACK ) ||
        ( side==BLACK && board.turn==WHITE )) {
      sleep(botmvdelay);
      int moveIndex = rand() % num_of_moves;
      // printf("(%d) ", moveIndex);
      move_t move = valid_moves[moveIndex];
      indexToName(move.startsqr, from);
      indexToName(move.targetsqr, to);
      printf("\nbot played: %s %s", from, to);
      board = makeMove(&board, move);
    }
    
    else {
      printf("Enter your move: ");
      while(1) {
        scanf("%s %s", from, to);
        move_t move = getLegalMoveby(nameToIndex(from), nameToIndex(to), valid_moves);

        if(move.promotingto!=NOPIECE) { // --- needs attention
          int promotionPiece = askPromotionPiece();
          move.promotingto = promotionPiece;
        }
        if(move.startsqr==-1) {
          printLegalMoves(valid_moves);
          printf("Invalid move!! Enter again: ");
        }
        else {
          board = makeMove(&board, move);
          break;
        }
      }
    }

    num_of_moves = update_legal_moves(board, valid_moves);
    printf("\n");
  }
  printf("FEN: ");
  printFen(board);
  printf("\n");
  drawBoard(board);
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
