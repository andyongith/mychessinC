#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BORDERCOLOR "\033[32;1m"
#define BLACKCOLOR "\033[31;1m"
#define WHITECOLOR "\033[37;1m"

enum Piece {
  EMPTY=' ',
  wKING='K',wPAWN='P',wKNIGHT='N',wBISHOP='B',wROOK='R',wQUEEN='Q',
  bKING='k',bPAWN='p',bKNIGHT='n',bBISHOP='b',bROOK='r',bQUEEN='q'
};

char Square[64];
char turn='w';

char startFen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
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

  char rowSeparator[] = "+---+---+---+---+---+---+---+---+";
  char colSeparator[] = "|";
  printf("  %s%s\033[0m\n", BORDERCOLOR, rowSeparator);

  for(int r=7; r>=0; r--) {
    printf("%s%d \033[0m", BORDERCOLOR, r+1);
    
    for(int c=0; c<8; c++) {
      printf("%s%s\033[0m", BORDERCOLOR, colSeparator);
      char letter = Square[r*8 + c];
      if('a'<=letter && letter<='z') printf(BLACKCOLOR);
      else printf(WHITECOLOR);
      printf(" %c \033[0m", letter);
    }

    printf("%s%s\033[0m\n", BORDERCOLOR, colSeparator);
    printf("  %s%s\033[0m\n", BORDERCOLOR, rowSeparator);
  }
  printf("%s    a   b   c   d   e   f   g   h\033[0m\n", BORDERCOLOR);
}


struct {
  int from; int to;
  int takes;
  char name[8];
} Move[256];

void numToName(int num, char* namePtr) {
  namePtr[0] = 'a' + (num%8);
  namePtr[1] = '1' + (num/8);
}

void moveFromTo(char* from, char* to) {
  int begn = (int) (from[0]-'a' + (from[1]-'1')*8);
  int dstn = (int) (  to[0]-'a' + (  to[1]-'1')*8);

  printf("(%s, %s)\n", from, to);

  Square[dstn] = Square[begn];
  Square[begn] = EMPTY;
}

void addMove(int index, int from, int to, int takes, char* name) {
  Move[index].from = from;
  Move[index].to = to;
  Move[index].takes = takes;
  strcpy(Move[index].name, name);
}

void calcMoves() {
  int moveno = 0;
  int isSameColor(int sqr, int color) { //white=0, black=1
    if(color==0) return 'A'<=Square[sqr] && Square[sqr]<='Z';
    if(color==1) return 'a'<=Square[sqr] && Square[sqr]<='z';
    return -1;
  }
  for(int i=0; i<64; i++) {
    enum Piece pc = Square[i];
    if(pc==EMPTY) continue;
    int itscolor = ('a'<=Square[i] && Square[i]<='z');

    int dests[28], destsI=0;
    if(pc==wROOK || pc==bROOK) {
      destsI=0;
      for(int dest=i+8; dest<64 && !isSameColor(dest,itscolor) && !isSameColor(dest-8,!itscolor);        dest+=8) dests[destsI++]=dest;
      for(int dest=i-8; dest>=0 && !isSameColor(dest,itscolor) && !isSameColor(dest+8,!itscolor);        dest-=8) dests[destsI++]=dest;
      for(int dest=i+1; dest<(i+8)/8*8 && !isSameColor(dest,itscolor) && !isSameColor(dest-1,!itscolor); dest+=1) dests[destsI++]=dest;
      for(int dest=i-1; dest>=i/8*8 && !isSameColor(dest,itscolor) && !isSameColor(dest+1,!itscolor);    dest-=1) dests[destsI++]=dest;
    }
    else if(pc==wBISHOP || pc==bBISHOP) {
      destsI=0;
      for(int dest=i+9; dest<64 && dest%8>(dest-9)%8 && !isSameColor(dest,itscolor) && !isSameColor(dest-9,!itscolor); dest+=9) dests[destsI++]=dest;
      for(int dest=i+7; dest<64 && dest%8<(dest-9)%8 && !isSameColor(dest,itscolor) && !isSameColor(dest-7,!itscolor); dest+=7) dests[destsI++]=dest;
      for(int dest=i-9; dest>=0 && dest%8<(dest-9)%8 && !isSameColor(dest,itscolor) && !isSameColor(dest+9,!itscolor); dest-=9) dests[destsI++]=dest;
      for(int dest=i-7; dest>=0 && dest%8>(dest-9)%8 && !isSameColor(dest,itscolor) && !isSameColor(dest+7,!itscolor); dest-=7) dests[destsI++]=dest;
    }
    else if(pc==wQUEEN || pc==bQUEEN) {
      destsI=0;
      for(int dest=i+8; dest<64 && !isSameColor(dest,itscolor) && !isSameColor(dest-8,!itscolor);        dest+=8) dests[destsI++]=dest;
      for(int dest=i-8; dest>=0 && !isSameColor(dest,itscolor) && !isSameColor(dest+8,!itscolor);        dest-=8) dests[destsI++]=dest;
      for(int dest=i+1; dest<(i+8)/8*8 && !isSameColor(dest,itscolor) && !isSameColor(dest-1,!itscolor); dest+=1) dests[destsI++]=dest;
      for(int dest=i-1; dest>=i/8*8 && !isSameColor(dest,itscolor) && !isSameColor(dest+1,!itscolor);    dest-=1) dests[destsI++]=dest;
      for(int dest=i+9; dest<64 && dest%8>(dest-9)%8 && !isSameColor(dest,itscolor) && !isSameColor(dest-9,!itscolor); dest+=9) dests[destsI++]=dest;
      for(int dest=i+7; dest<64 && dest%8<(dest-9)%8 && !isSameColor(dest,itscolor) && !isSameColor(dest-7,!itscolor); dest+=7) dests[destsI++]=dest;
      for(int dest=i-9; dest>=0 && dest%8<(dest-9)%8 && !isSameColor(dest,itscolor) && !isSameColor(dest+9,!itscolor); dest-=9) dests[destsI++]=dest;
      for(int dest=i-7; dest>=0 && dest%8>(dest-9)%8 && !isSameColor(dest,itscolor) && !isSameColor(dest+7,!itscolor); dest-=7) dests[destsI++]=dest;
    }
    else if(pc==wKNIGHT || pc==bKNIGHT) {
      destsI=0;
      if(i+6 <64 && i+6 >=0 && (i+6 )%8<i%8 && !isSameColor(i+6 ,itscolor)) dests[destsI++]=i+6;
      if(i+15<64 && i+15>=0 && (i+15)%8<i%8 && !isSameColor(i+15,itscolor)) dests[destsI++]=i+15;
      if(i-10<64 && i-10>=0 && (i-10)%8<i%8 && !isSameColor(i-10,itscolor)) dests[destsI++]=i-10;
      if(i-17<64 && i-17>=0 && (i-17)%8<i%8 && !isSameColor(i-17,itscolor)) dests[destsI++]=i-17;
      if(i+10<64 && i+10>=0 && (i+10)%8>i%8 && !isSameColor(i+10,itscolor)) dests[destsI++]=i+10;
      if(i+17<64 && i+17>=0 && (i+17)%8>i%8 && !isSameColor(i+17,itscolor)) dests[destsI++]=i+17;
      if(i-6 <64 && i-6 >=0 && (i-6 )%8>i%8 && !isSameColor(i-6 ,itscolor)) dests[destsI++]=i-6 ;
      if(i-15<64 && i-15>=0 && (i-15)%8>i%8 && !isSameColor(i-15,itscolor)) dests[destsI++]=i-15;
    }
    else if(pc==wKING || pc==bKING) {
      destsI=0;
      if(i-8>=0 && !isSameColor(i-8 ,itscolor)) dests[destsI++] = i-8;
      if(i+8<64 && !isSameColor(i+8 ,itscolor)) dests[destsI++] = i+8;
      if(i+1<(i+8)/8*8 && !isSameColor(i+1 ,itscolor)) dests[destsI++] = i+1;
      if(i-1>=i/8*8 && !isSameColor(i-1 ,itscolor))    dests[destsI++] = i-1;
      if(i+7<64 && (i+7)%8<i%8 && !isSameColor(i+7 ,itscolor)) dests[destsI++] = i+7;
      if(i-9>=0 && (i-9)%8<i%8 && !isSameColor(i-9 ,itscolor)) dests[destsI++] = i-9;
      if(i+9<64 && (i+9)%8>=i%8 && !isSameColor(i+9 ,itscolor)) dests[destsI++] = i+9;
      if(i-7>=0 && (i-7)%8>=i%8 && !isSameColor(i-7 ,itscolor)) dests[destsI++] = i-7;
    }
    else if(pc==wPAWN || pc==bPAWN) {
// needs attention
    }
    for(int j=0; j<destsI; j++) {
      char name[3];
      name[0] = itscolor ? pc - ('a'-'A') : pc;
      numToName(dests[j], name+1);
      addMove(moveno++, i, dests[j], dests[j], name);
    }
  }

  while(moveno<=256) {
    Move[moveno].from=-1;
    Move[moveno].to=-1;
    strcpy(Move[moveno].name, "X");
    moveno++;
  }
}


int main() {
  setPos(startFen);
  //setPos("7k/3N2qp/b5r1/2p1Q1N1/Pp4PK/7P/1P3p2/6r1 w - - 7 4");
  //setPos("8/8/8/3R4/8/8/8/8 w - -");
  //setPos("8/8/8/4K3/8/8/8/8 w - -");
  drawBoard();

  calcMoves();

  //moveFromTo("d2", "d4");
  //drawBoard();

  while(1) {
    printf("Available_Moves: ");
    for(int i=0; i<256; i++) {
      if(Move[i].from==-1) continue;
      printf("%s ", Move[i].name);
    }
    char from[10], to[10];
    printf("\nEnter you move: ");
    scanf("%s %s", &from, &to);
    moveFromTo(from, to);
    calcMoves();
    // system("clear");
    drawBoard();
  }

  return 0;
}
