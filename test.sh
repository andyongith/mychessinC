#!/bin/bash

set -e

testfens=(
  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"
  "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"
  "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"
  "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"
)

depth=2
if [[ -n $1 ]]; then
  depth=$1
fi
echo -e "DEPTH = ${depth}\n"

test_fen() {
  local fen=$1
  engine_output=$(echo -e "position fen ${fen}\ngo perft $depth" | stockfish | tail -n 2 | head -n 1 | cut -d ' ' -f 3)
  printf "%s %7d : " $2 ${engine_output}
  your_output=$(./start -f "${fen}" -t $depth)
  printf "%7d =>  " ${your_output}

  if [[ $engine_output -eq $your_output ]]; then
    echo "PASSED ✅"
  else
    echo "FAILED ❌"
    echo -e "\tfen: ${fen}\n"
  fi
}

# echo "Index) [engine output] : [your output] => result"
if [[ -n $2 ]]; then
  test_fen "$2" " "
else
  for i in "${!testfens[@]}"; do
    test_fen "${testfens[$i]}" " " # $i
  done
fi
