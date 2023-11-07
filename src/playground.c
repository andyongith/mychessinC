#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "playground.h"
#include "miscfunctions.h"
#include "position.h"
#include "display.h"
#include "legalmoves.h"

move_t ask_bot(move_t* valid_moves, int num_of_moves) {
  int moveIndex = rand() % num_of_moves;
  move_t move = valid_moves[moveIndex];
  return move;
}

move_t ask_bot_n_print(move_t* valid_moves, int num_of_moves) {
  move_t move = ask_bot(valid_moves, num_of_moves);

  char from[3], to[3];
  indexToName(move.startsqr, from);
  indexToName(move.targetsqr, to);
  printf("\nbot played: %s %s", from, to);
  return move;
}

move_t ask_player(move_t* valid_moves) {
  move_t move;
  printf("Enter your move: ");
  char from[3], to[3];
  while(1) {
    scanf("%s %s", from, to);
    move = getLegalMoveby(nameToIndex(from), nameToIndex(to), valid_moves);

    if(move.promotingto!=NOPIECE) { // --- needs attention
      int promotionPiece = askPromotionPiece();
      move.promotingto = promotionPiece;
    }
    if(move.startsqr==-1) {
      printLegalMoves(valid_moves);
      printf("Invalid move!! Enter again: ");
    }
    else {
      break;
    }
  }
  return move;
}

int askPromotionPiece() {
  int piece = QUEEN;
  printf("Promoting to?(Q,R,B,N) ");
  char piecesym;
  while(1) {
    scanf("%s", &piecesym);
    piece = piece_sym(piecesym);
    if(piece == NOPIECE || piece == ' ')
      printf("Invalid piece!! Enter again: ");
    else break;
  }
  return typeofpiece(piece);
}

void show_ending_msg(int num_of_moves) {
  switch(num_of_moves) {
    case  0: printf("\033[31m\"Checkmate\"\033[30m\n"); break;
    case -1: printf("\033[31m\"Stalemate\"\033[30m\n"); break;
    case -2: printf("\033[31m\"Draw by 50 move rule\"\033[30m\n"); break;
    default: printf("\033[31m\"Something's wrong, and idk what.\"\033[30m\n"); break;
  }
}

void play_randomly(board_t board) {
  move_t valid_moves[MOVES_ARR_LEN];
  int num_of_moves = update_legal_moves(board, valid_moves);

  while(num_of_moves>0) {
    drawBoard(board, enable_unicode);
    printf("FEN: ");
    printFen(board);
    printf("\n");
    if(showlegals) {
      printLegalMoves(valid_moves);
      printf("num_of_available_moves: %d\n", num_of_moves);
    }
    
    sleep(botmvdelay);
    move_t move = ask_bot_n_print(valid_moves, num_of_moves);

    num_of_moves = update_legal_moves(board, valid_moves);
  }
  drawBoard(board, enable_unicode);
  printf("FEN: ");
  printFen(board);
  printf("\n");
  show_ending_msg(num_of_moves);
}

void play_manually(int side, board_t board) { // side can be WHITE, BLACK or any other number for both
  move_t valid_moves[MOVES_ARR_LEN];
  int num_of_moves = update_legal_moves(board, valid_moves);

  while(num_of_moves>0) {
    drawBoard(board, enable_unicode);
    printf("FEN: ");
    printFen(board);
    printf("\n");
    if(showlegals) {
      printLegalMoves(valid_moves);
      printf("num_of_available_moves: %d\n", num_of_moves);
    }
    
    if(( side==WHITE && board.turn==BLACK ) ||
        ( side==BLACK && board.turn==WHITE )) {
      sleep(botmvdelay);
      move_t move = ask_bot_n_print(valid_moves, num_of_moves);

      board = makeMove(&board, move);
    }
    
    else {
      move_t move = ask_player(valid_moves);
      board = makeMove(&board, move);
    }

    num_of_moves = update_legal_moves(board, valid_moves);
    printf("\n");
  }
  drawBoard(board, enable_unicode);
  printf("FEN: ");
  printFen(board);
  printf("\n");
  show_ending_msg(num_of_moves);
}
