#ifndef LEGAL_MOVES_H
#define LEGAL_MOVES_H

typedef struct{
  int startsqr;
  int targetsqr;
  int captures;
  bool is_en_passant_pawn;
} Move;

void numToName(int num, char name[3]);
int nameToNum(char name[3]);

void calculate_moves();
Move getMoveby(int from, int to);
void makeMove(Move move);

void printLegalMoves();

#endif // LEGAL_MOVES_H
