#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <random.h>
#include <time.h>
#include <unistd.h>

#include "miscfunctions.h"
#include "position.h"
#include "display.h"
#include "legalmoves.h"

int main() {
  char testFens[30][64];
  strcpy(testFens[ 0], STARTFEN);
  strcpy(testFens[ 1], "7k/3N2qp/b5r1/2p1Q1N1/Pp4PK/7P/1P3p2/6r1 w - - 7 4");
  strcpy(testFens[ 2], "8/1r4r1/8/8/8/8/1R4R1/8 w - - 0 1");
  strcpy(testFens[ 3], "8/8/8/8/3R4/8/8/8 w - - 0 1");
  strcpy(testFens[ 4], "8/8/8/8/3B4/8/8/8 w - - 0 1");
  strcpy(testFens[ 5], "8/8/8/8/3Q4/8/8/8 w - - 0 1");
  strcpy(testFens[ 6], "8/6B1/5P2/4P3/3K4/8/8/8 w - - 0 1");
  strcpy(testFens[ 7], "8/8/8/8/3n4/8/8/8 w - - 0 1");
  strcpy(testFens[ 8], "7k/3N2qp/b5r1/2p1Q1N1/Pp4PK/2r4P/1P3p2/6r1 w - - 7 4");
  strcpy(testFens[ 9], "8/1P7/8/4k3/8/3K4/7p/8 w - - 0 1");
  strcpy(testFens[10], "rn2k2r/pppppppp/8/8/8/8/PPPPPPPP/Rn2K1nR/ w KQkq - 0 1");
  strcpy(testFens[11], "k7/8/2q5/8/3K4/8/8/8 w - - 0 1");
  strcpy(testFens[12], "8/8/8/8/3K4/8/8/8 w - - 0 1");
  strcpy(testFens[13], "8/8/8/4P3/8/8/8/8 w - - 0 1");
  strcpy(testFens[14], "8/5b2/8/4k3/8/r2R1K2/8/3n4 w - - 0 1");
  strcpy(testFens[15], "8/3r4/8/3R4/8/8/3K4/8 w - - 0 1");
  strcpy(testFens[16], "1r3b1k/rN6/1R1N4/8/1K3P1r/8/3B4/4b3 w - - 0 1");
  strcpy(testFens[17], "r3k2r/8/8/8/8/8/8/R3K2R/ w KQkq - 0 1");
  strcpy(testFens[18], "8/3p4/8/1KP4r/8/8/4k3/8 b - - 0 1");
  strcpy(testFens[19], "3B4/8/8/3K3r/3N4/8/4k3/3q4 w - - 0 1");

  board_t Board;
  move_t valid_moves[MOVES_ARR_LEN];

  initPositionVars(&Board);
  setPosition(testFens[ 0], &Board);
  drawBoard(&Board);
  int num_of_moves = update_legal_moves(&Board, valid_moves);

  srand(time(NULL));
  while(num_of_moves>0) {
    // printLegalMoves(valid_moves);
    // printf("Board.castle => ");
    // if(Board.castle & WHITE_KING_SIDE ) printf("K");
    // if(Board.castle & WHITE_QUEEN_SIDE) printf("Q");
    // if(Board.castle & BLACK_KING_SIDE ) printf("k");
    // if(Board.castle & BLACK_QUEEN_SIDE) printf("q");
    // printf("\n");

    char from[3], to[3];
    printf("Enter your move: ");
    
    if(Board.turn==WHITE) {
      while(1) {
        scanf("%s %s", from, to);
        move_t move = getLegalMoveby(nameToIndex(from), nameToIndex(to), &Board, valid_moves);
        if(move.startsqr==-1)
          printf("Invalid move!! Enter again: ");
        else {
          makeMove(&Board, move);
          break;
        }
      }
    }
    else {
      sleep(1);
      int moveIndex = rand() % num_of_moves;
      // printf("(%d) ", moveIndex);
      move_t move = valid_moves[moveIndex];
      indexToName(move.startsqr, from);
      indexToName(move.targetsqr, to);
      printf("%s %s\n", from, to);
      makeMove(&Board, move);
    }
    //

    drawBoard(&Board);
    num_of_moves = update_legal_moves(&Board, valid_moves);
    // sleep(1);
  }

  return 0;
}
