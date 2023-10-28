#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "miscfunctions.h"
#include "position.h"
#include "display.h"
#include "legalmoves.h"

int showlegals=0;
int botmvdelay=0;

board_t Board;
move_t valid_moves[MOVES_ARR_LEN];

void play_randomly();
void play_manually(int side);

int askPromotionPiece();

void showNodesNum(board_t board, int depth) {
}

int main(int argc, char **argv) {
  char testFens[30][64];
  strcpy(testFens[ 0], STARTFEN);

  int feni=0;
  strcpy(testFens[++feni], "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
  strcpy(testFens[++feni], "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
  strcpy(testFens[++feni], "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
  strcpy(testFens[++feni], "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
  strcpy(testFens[++feni], "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

  initPositionVars(&Board);

  char loadingfen[64] = STARTFEN;
  bool noBoard = false;

  int argi = 1;
  while(argi<argc && argv[argi][0]=='-') {
    switch(argv[argi][1]) {
      case 'f':
             if(strcmp(argv[argi+1], "startfen")==0) strcpy(loadingfen, STARTFEN);
        else if(strcmp(argv[argi+1], "ltestfen")==0) strcpy(loadingfen, testFens[feni]);
        else if(strcmp(argv[argi+1], "testfen" )==0) strcpy(loadingfen, testFens[stringtonum(argv[(argi++)+2])]);
        else strcpy(loadingfen, argv[argi+1]);
        argi++;
        break;

      case 't':
        printf("\"-t\" option is for testing purposes only.\n");
        noBoard = true;
        return 2;
        break;

      case 'h':
        printf("OPTIONS\n");
        printf("-h\tshows this help.\n");
        printf("\n");

        printf("-f <sub_option>\tloads custom fen.\n");
        printf("\t\"-f <your_fen_inside_quotes>\"\tloads your custom position.\n");
        printf("\t\"-f startfen\"\tloads the starting position.\n");
        printf("\t\"-f testfen <num>\"\tloads <num>th testfen.\n");
        printf("\t\"-f ltestfen\"\tloads last testfen.\n");
        printf("\n");

        printf("-t\tI added this option for testing purposes only.\n");

        return 0;
        break;

      default:
        printf("Invalid option\nEnter \"./start -h\" for help and more\n");
        return 1;
    }
    argi++;
  }

  setPosition(loadingfen, &Board);

  srand(time(NULL));
  if(!noBoard && argi < argc) {
         if(strcmp(argv[argi], "white" )==0) play_manually(WHITE);
    else if(strcmp(argv[argi], "black" )==0) play_manually(BLACK);
    else if(strcmp(argv[argi], "both"  )==0) play_manually(0);
    else if(strcmp(argv[argi], "random")==0) play_randomly();
  }
  else if(!noBoard) play_manually(WHITE);

  return 0;
}

void play_randomly() {
  int num_of_moves = update_legal_moves(&Board, valid_moves);

  while(num_of_moves>0) {
    printFen(Board);
    drawBoard(Board);
    printf("num_of_available_moves: %d\n", num_of_moves);
    if(showlegals) printLegalMoves(valid_moves);
    
    sleep(botmvdelay);
    char from[3], to[3];
    int moveIndex = rand() % num_of_moves;
    // printf("(%d) ", moveIndex);
    move_t move = valid_moves[moveIndex];
    indexToName(move.startsqr, from);
    indexToName(move.targetsqr, to);
    printf("\nbot played: %s %s\n", from, to);
    makeMove(&Board, move);

    num_of_moves = update_legal_moves(&Board, valid_moves);
    // sleep(botmvdelay);
  }
  printFen(Board);
  drawBoard(Board);
}

void play_manually(int side) { // side can be WHITE, BLACK or any other number for both
  int num_of_moves = update_legal_moves(&Board, valid_moves);

  while(num_of_moves>0) {
    printFen(Board);
    drawBoard(Board);
    printf("num_of_available_moves: %d\n", num_of_moves);
    if(showlegals) printLegalMoves(valid_moves);
    
    char from[3], to[3];

    if(( side==WHITE && Board.turn==BLACK ) ||
        ( side==BLACK && Board.turn==WHITE )) {
      sleep(botmvdelay);
      int moveIndex = rand() % num_of_moves;
      // printf("(%d) ", moveIndex);
      move_t move = valid_moves[moveIndex];
      indexToName(move.startsqr, from);
      indexToName(move.targetsqr, to);
      printf("\nbot played: %s %s", from, to);
      makeMove(&Board, move);
    }
    
    else {
      printf("Enter your move: ");
      while(1) {
        scanf("%s %s", from, to);
        move_t move = getLegalMoveby(nameToIndex(from), nameToIndex(to), &Board, valid_moves);

        if(move.promotingto!=NOPIECE) { // --- needs attention
          int promotionPiece = askPromotionPiece();
          move.promotingto = promotionPiece;
        }
        if(move.startsqr==-1) {
          printLegalMoves(valid_moves);
          printf("Invalid move!! Enter again: ");
        }
        else {
          makeMove(&Board, move);
          break;
        }
      }
    }

    num_of_moves = update_legal_moves(&Board, valid_moves);
    printf("\n");
  }
  printFen(Board);
  drawBoard(Board);
}

int askPromotionPiece() {
  int piece = QUEEN;
  printf("Promoting to?(Q,R,B,N) ");
  char piecesym;
  while(1) {
    scanf("%s", &piecesym);
    piece = piece_sym[piecesym];
    if(piece == NOPIECE || piece == ' ')
      printf("Invalid piece!! Enter again: ");
    else break;
  }
  return typeofpiece(piece);
}
