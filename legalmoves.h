#ifndef LEGAL_MOVES_H
#define LEGAL_MOVES_H

#define MOVES_ARR_LEN 256

typedef struct{
  int startsqr;
  int targetsqr;
  int captures;
  bool is_en_passant_pawn;
  bool is_castling;
  bool promotion;
} move_t;

void update_legal_moves(board_t* board, move_t* moves);
move_t getLegalMoveby(int from, int to, board_t* board, move_t* moves);
void makeMove(board_t* board, move_t move);

#endif // LEGAL_MOVES_H
