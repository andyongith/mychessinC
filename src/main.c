#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "miscfunctions.h"
#include "position.h"
#include "display.h"
#include "legalmoves.h"
#include "playground.h"

int showlegals=0;
int botmvdelay=1;

board_t Board;

int showNodesNum(board_t board, int depth) {
  move_t moves[MOVES_ARR_LEN];
  int moves_num = update_legal_moves(board, moves);
  return moves_num;
}

int main(int argc, char **argv) {
  char testFens[30][FEN_LEN];
  strcpy(testFens[ 0], STARTFEN);

  int feni=0;
  strcpy(testFens[++feni], "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
  strcpy(testFens[++feni], "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
  strcpy(testFens[++feni], "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
  strcpy(testFens[++feni], "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
  strcpy(testFens[++feni], "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

  initPositionVars(&Board);

  char loadingfen[FEN_LEN] = STARTFEN;
  bool noBoard = false;
  bool testing = false;

  int argi = 1;
  while(argi<argc && argv[argi][0]=='-') {
    switch(argv[argi][1]) {
      case 'h':
        printf("OPTIONS\n");
        printf("-h => shows this help.\n");
        printf("\n");

        printf("-f <sub_option> => loads custom fen.\n");
        printf("-f <your_fen_inside_quotes> => loads your custom position.\n");
        printf("-f startfen => loads the starting position.\n");
        printf("-f testfen <num> => loads <num>th testfen.\n");
        printf("-f ltestfen => loads last testfen.\n");
        printf("\n");

        printf("-t => I added this option for testing purposes only.\n");
        printf("\n");

        printf("-l => Forces to always show legal moves.\n");
        printf("\n");

        printf("-d <num> => Sets delay for each bot move. Default is 1sec.\n");

        return 0;
        break;

      case 'f':
             if(strcmp(argv[argi+1], "startfen")==0) strcpy(loadingfen, STARTFEN);
        else if(strcmp(argv[argi+1], "ltestfen")==0) strcpy(loadingfen, testFens[feni]);
        else if(strcmp(argv[argi+1], "testfen" )==0) strcpy(loadingfen, testFens[stringtonum(argv[(argi++)+2])]);
        else strcpy(loadingfen, argv[argi+1]);
        argi++;
        break;

      case 't':
        noBoard = true;
        testing = true;
        break;

      case 'l':
        showlegals = true;
        break;

      case 'd':
        botmvdelay = stringtonum(argv[(argi++)+1]);
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
         if(strcmp(argv[argi], "white" )==0) play_manually(WHITE, Board);
    else if(strcmp(argv[argi], "black" )==0) play_manually(BLACK, Board);
    else if(strcmp(argv[argi], "both"  )==0) play_manually(0, Board);
    else if(strcmp(argv[argi], "random")==0) play_randomly(Board);
  }
  else if(!noBoard) play_manually(WHITE, Board);

  if(testing) {
    printf("%d\n", showNodesNum(Board, 1));
  }

  return 0;
}