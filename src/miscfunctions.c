#include <string.h>

#include "position.h"
#include "miscfunctions.h"

void indexToName(int num, char name[3]) {
  if(num==-1) strcpy(name, "  ");
  else {
    name[0] = num%8 + 'a';
    name[1] = num/8 + '1';
    name[2] = '\0';
  }
}

int nameToIndex(char name[3]) {
  if(name[0]<'a' || name[0]>'z' || name[1]<'1' || name[1]>'8') return 0;
  int col = name[0]-'a';
  int row = name[1]-'1';
  return row*8 + col;
}

int colorofpiece(int piece) {
  return piece & 24; // 24 = WHITE | BLACK
}

int typeofpiece(int piece) {
  return piece & 7; // 7 = KING | QUEEN | KNIGHT | ROOK | BISHOP
}

int oppositecolor(int color) {
  return color ^ 24; // 24 = WHITE | BLACK
}

int searchIn(int array[], int len, int target) {
  int index = -1;
  for(int i=0; i<len; i++) {
    if(array[i] == target) return i;
  }
  return index;
}

int stringtonum(char *string) {
  int digits = strlen(string);
  int num=0, i=0;
  if(string[0]=='-') i=1;
  while(i<digits) {
    num *= 10;
    num += string[i++] - '0';
  }
  return num;
}
