#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "position.h"
#include "miscfunctions.h"
#include "display.h"

#define BORDERCOLOR "\033[32;1m"
#define BLACKCOLOR "\033[31;1m"
#define WHITECOLOR "\033[37;1m"
#define RESETCOLOR "\033[0m"

char rowSeparator[] = "+---+---+---+---+---+---+---+---+";
char colSeparator[] = "|";

// char unicodeSym[] = "   "; int unicodeSize = 3;
char unicodeSym[] = "    󰡗󰡙󰡘󰡜󰡛󰡚"; int unicodeSize = 4;

void drawBoard(board_t board, bool enable_unicode) {
  printf("  %s%s\n", BORDERCOLOR, rowSeparator);

  int row=7, col=0;
  while(row>=0) {
    if(col==0) {
      printf("%s%d |", BORDERCOLOR, row+1);
    }

    int piece = board.squares[row*8 + col];

    printf("%s ", piece & WHITE ? WHITECOLOR : BLACKCOLOR);
    if(typeofpiece(piece)==NOPIECE) printf(" ");
    else if(enable_unicode) {
      int index = unicodeSize * typeofpiece(piece);
      for(int i=0; i<unicodeSize; i++)
        printf("%c", unicodeSym[index+i]);
    }
    else printf("%c", (char) piece_sym(piece));
    printf(" %s%s", BORDERCOLOR, colSeparator);

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
void printSquares(board_t board) {
  for(int i=0; i<64; i++) {
    printf("%c,", piece_sym(board.squares[i]));
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

void printFen(board_t board) {
  char fen[FEN_LEN];
  int i=0;

  int letters1[] = {'k','p','n','b','r','q'};
  int letters2[] = {'K','P','N','B','R','Q'};
  int values[] = {KING, PAWN, KNIGHT, BISHOP, ROOK, QUEEN};
  for(int r=7; 0 <= r; r--) {
    int skipped=0;
    for(int c=0; c < 8; c++) {
      int index = r*8 + c;
      int piece = board.squares[index];
      
      if(piece == NOPIECE) {
        skipped++;
      }
      else if(skipped>0) {
        fen[i++] = skipped + '0';
        skipped=0;
      }

      char pieceSym='1';
      switch(colorofpiece(piece)) {
        case WHITE:
          for(int j=0; j<8; j++) {
            if(piece != (WHITE | values[j])) continue; 
            pieceSym = letters2[j];
            break;
          }
          fen[i] = pieceSym;
          i++; break;
        case BLACK:
          for(int j=0; j<8; j++) {
            if(piece != (BLACK | values[j])) continue; 
            pieceSym = letters1[j];
            break;
          }
          fen[i] = pieceSym;
          i++; break;
      }
    }
    if(skipped>0) fen[i++] = skipped + '0';
    if(r!=0) fen[i++] = '/';
  }

  fen[i++] = ' ';
  fen[i++] = (board.turn == WHITE) ? 'w' : 'b';

  fen[i++] = ' ';
  if(board.castle == 0) fen[i++] = '-';
  else {
    if(board.castle & WHITE_KING_SIDE ) fen[i++] = 'K';
    if(board.castle & WHITE_QUEEN_SIDE) fen[i++] = 'Q';
    if(board.castle & BLACK_KING_SIDE ) fen[i++] = 'k';
    if(board.castle & BLACK_QUEEN_SIDE) fen[i++] = 'q';
  }

  fen[i++] = ' ';
  if(board.en_passant_pawn == -1) fen[i++] = '-';
  else {
    indexToName(board.en_passant_pawn, fen+i);
    i += 2;
  }
  // fen is incomplete here but we don't need to complete it

  fen[i] = '\0';
  printf("%s %d %d", fen, board.halfmove, board.fullmove);
}
