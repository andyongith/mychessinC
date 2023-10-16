#include <stdio.h>
#include <string.h>

#include "position.h"
#include "miscfunctions.h"
#include "display.h"

#define BORDERCOLOR "\033[32;1m"
#define BLACKCOLOR "\033[31;1m"
#define WHITECOLOR "\033[37;1m"
#define RESETCOLOR "\033[0m"

char rowSeparator[] = "+---+---+---+---+---+---+---+---+";
char colSeparator[] = "|";

void drawBoard(board_t* board) {
  printf("  %s%s\n", BORDERCOLOR, rowSeparator);

  int row=7, col=0;
  while(row>=0) {
    if(col==0) {
      printf("%s%d |", BORDERCOLOR, row+1);
    }

    int piece = board->square[row*8 + col];

    printf("%s", piece & WHITE ? WHITECOLOR : BLACKCOLOR);
    printf(" %c ", (char) piece_sym[piece]);
    printf("%s%s", BORDERCOLOR, colSeparator);

    if(col==7) {
      printf("%s\n  %s\n", BORDERCOLOR, rowSeparator);
      row--;
      col=0;
    }
    else col++;
  }
  printf("%s    a   b   c   d   e   f   g   h%s\n", BORDERCOLOR, RESETCOLOR);
}

// Only For debugging(will remove it later)(don't forget to also remove from header)
void printSquares(board_t* board) {
  for(int i=0; i<64; i++) {
    printf("%c,", piece_sym[board->square[i]]);
  }
}

void printLegalMoves(move_t* moves) {
  printf("----Legal Moves----\n");

  for(int i=0; i<MOVES_ARR_LEN && moves[i].startsqr!=-1; i++) {
    char sqr1[] = "z9", sqr2[] = "z9", sqr3[] = "z9";
    indexToName(moves[i].startsqr, sqr1);
    indexToName(moves[i].targetsqr, sqr2);
    indexToName(moves[i].captures, sqr3);

    // printf("(%s,%s,{%s}%d)\t", sqr1, sqr2, sqr3, moves[i].is_castling);
    printf("(%s %s)\t", sqr1, sqr2);
    // printf("%s ", sqr2);
  }
  printf("\n");
}
