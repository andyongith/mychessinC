#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "position.h"
#include "display.h"
#include "legalmoves.h"

int main() {
  initPositionVars();
  setPosition(STARTFEN);

  drawBoard();

  // printSquares();

  return 0;
}
