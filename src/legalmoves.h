#ifndef LEGAL_MOVES_H
#define LEGAL_MOVES_H

#define MOVES_ARR_LEN 256

typedef struct{
  int startsqr;
  int targetsqr;
  int captures;
  bool is_en_passant_pawn;
  bool is_castling;
  int promotingto;
} move_t;

int update_legal_moves(board_t board, move_t* moves);
move_t getLegalMoveby(int from, int to, move_t* moves);
board_t makeMove(board_t* board, move_t move);

#endif // LEGAL_MOVES_H
