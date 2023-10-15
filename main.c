#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "miscfunctions.h"
#include "position.h"
#include "display.h"
#include "legalmoves.h"

int main() {
  char testFens[20][64];
  strcpy(testFens[0], STARTFEN);
  strcpy(testFens[1], "7k/3N2qp/b5r1/2p1Q1N1/Pp4PK/7P/1P3p2/6r1 w - - 7 4");
  strcpy(testFens[2], "8/1r4r1/8/8/8/8/1R4R1/8 w - - 0 1");
  strcpy(testFens[3], "8/8/8/8/3R4/8/8/8 w - - 0 1");
  strcpy(testFens[4], "8/8/8/8/3b4/8/8/8 w - - 0 1");
  strcpy(testFens[5], "8/8/8/8/3Q4/8/8/8 w - - 0 1");
  strcpy(testFens[6], "8/8/8/4p3/3k4/8/8/8 w - - 0 1");
  strcpy(testFens[7], "8/8/8/8/3n4/8/8/8 w - - 0 1");
  strcpy(testFens[8], "7k/3N2qp/b5r1/2p1Q1N1/Pp4PK/2r4P/1P3p2/6r1 w - - 7 4");
  strcpy(testFens[9], "8/1P7/8/4k3/8/3K4/7p/8 w - - 0 1");
  strcpy(testFens[10], "rn2k2r/pppppppp/8/8/8/8/PPPPPPPP/Rn2K1nR/ w KQkq - 0 1");

  board_t Board;
  move_t valid_moves[MOVES_ARR_LEN];

  initPositionVars(&Board);
  // setPosition(STARTFEN, &Board);
  setPosition(testFens[0], &Board);
  drawBoard(&Board);

  // update_legal_moves(valid_moves, &BoardBoard, valid_moves);
  // move_t firstMove = getLegalMoveby(11, 19, &Board);
  // makeMove(firstMove, &Board);
  // drawBoard(&Board);

  update_legal_moves(&Board, valid_moves);

  while(1) {
    // printLegalMoves(&Board);
    // printf("Board.castle => %d\n", Board.castle);

    char from[3], to[3];
    printf("Enter your move: ");
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

    drawBoard(&Board);
    update_legal_moves(&Board, valid_moves);
  }

  // printSquares(&Board);
  // printLegalMoves(&Board);

  return 0;
}
