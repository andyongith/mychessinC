#include <stdio.h>
#include <string.h>

#include "position.h"
#include "display.h"

#define BORDERCOLOR "\033[32;1m"
#define BLACKCOLOR "\033[31;1m"
#define WHITECOLOR "\033[37;1m"
#define RESETCOLOR "\033[0m"

char rowSeparator[] = "+---+---+---+---+---+---+---+---+";
char colSeparator[] = "|";

void drawBoard() {
  printf("  %s%s\n", BORDERCOLOR, rowSeparator);

  int row=7, col=0;
  while(row>=0) {
    if(col==0) {
      printf("%s%d |", BORDERCOLOR, row+1);
    }

    char sqr = (char) piece_sym[Board.square[row*8 + col]];
    printf(" %c ", sqr);
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
