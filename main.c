#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum Piece {
  EMPTY=' ',
  wKING='K',wPAWN='P',wKNIGHT='N',wBISHOP='B',wROOK='R',wQUEEN='Q',
  bKING='k',bPAWN='p',bKNIGHT='n',bBISHOP='b',bROOK='r',bQUEEN='q'
};

static char Square[64];
static char turn='w';

static char startFen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
void setPos(char* fen) {
  int fenlen=strlen(fen), i=0;
  
  // Board Setup
  int r=7, c=0;
  while(i<fenlen && fen[i]!=' ') {
    if(fen[i]=='/') {
      r--;
      c=0;
    }
    else if('0'<=fen[i] && fen[i]<='9') {
      int skipc = (int) (fen[i]-'0');
      while(skipc--) {
        Square[r*8 + c]=EMPTY;
        c++;
      }
    }
    else {
      Square[r*8 + c]=fen[i];
      c++;
    }
    i++;
  }

  turn = fen[i];
  i+=2;
}

void drawBoard() {
  char rowsBorder[] = "+---+---+---+---+---+---+---+---+";
  printf("%s\n", rowsBorder);
  for(int r=7; r>=0; r--) {
    for(int c=0; c<8; c++)
      printf("| %c ", Square[r*8 + c]);
    printf("|\n");
    printf("%s\n", rowsBorder);
  }
}

static struct {
  int from; int to;
  char name[8];
} Move[256];

void moveFromTo(char* from, char* to) {
  int begn = (int) (from[0]-'a' + (from[1]-'1')*8);
  int dstn = (int) (  to[0]-'a' + (  to[1]-'1')*8);

  printf("(%s, %s)\n", from, to);

  Square[dstn] = Square[begn];
  Square[begn] = EMPTY;
}

void calcMoves() {
  int moveno = 0;
  for(int i=0; i<64; i++) {
    
  }

  while(moveno<=256) {
    Move[moveno].from=-1;
    Move[moveno].to=-1;
    strcpy(Move[moveno].name, "X");
    moveno++;
  }
}


int main() {
  //setPos(startFen);
  setPos("7k/3N2qp/b5r1/2p1Q1N1/Pp4PK/7P/1P3p2/6r1 w - - 7 4");
  //setPos("8/8/8/3R4/8/8/8/8 w - -");
  drawBoard();

  //moveFromTo("d2", "d4");
  //drawBoard();

  while(1) {
    char from[10], to[10];
    printf("\nEnter you move: ");
    scanf("%s %s", &from, &to);
    moveFromTo(from, to);
    system("clear");
    drawBoard();
  }

  return 0;
}
