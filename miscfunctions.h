#ifndef SOME_MISCELLANEOUS_FUNCTIONS
#define SOME_MISCELLANEOUS_FUNCTIONS

void indexToName(int num, char name[3]);
int nameToIndex(char name[3]);

int colorofpiece(int piece);
int typeofpiece(int piece);
int oppositecolor(int color);

int searchIn(int array[], int len, int target);
int stringtonum(char *string);

#endif
