#!/bin/bash
set -e

# Scripts own path
scriptpath=$(dirname "$(readlink -f $0)")

if ! [[ -f ${scriptpath}/../start ]]
then
  echo ":( sorry dude ../start file doesn't exist"
  
  shouldCompile='n'
  echo -n ":) Should I complie your program?(y/N): "
  read shouldCompile
  if [[ $shouldCompile == [yY] ]]
  then
    make -C ${scriptpath}/..
  else
    exit
  fi
fi

depth=1
tempfen=
if [[ -f "${scriptpath}/testfens" ]]; then
  fenfile="${scriptpath}/testfens"
else
  fenfile="${scriptpath}/defTestFens"
fi

argSelected=
for arg in "$@"
do
  case $argSelected in
    "depth")
      depth=${arg}
      ;;
    "tempfen")
      tempfen=${arg}
      ;;
    "in_file")
      fenfile=${arg}
      ;;
  esac
  argSelected=

  case $arg in
    "-d")
      argSelected="depth"
      ;;
    "-F")
      argSelected="tempfen"
      ;;
    "-f")
      argSelected="in_file"
      ;;
  esac
done

echo -e "DEPTH = ${depth}\n"

test_fen() {
  IFS=';'
  local fen=
  read -ra fen <<< "$1"
  local fen1=${fen[0]}
  local fen2=${fen[-1]}

  engine_output=$(echo -e "position fen ${fen2}\ngo perft $depth" | stockfish | tail -n 2 | head -n 1 | cut -d ' ' -f 3)
  printf "%s %7d : " $2 ${engine_output}

  your_output=$(${scriptpath}/../start -f "${fen1}" -t $depth)
  printf "%7d =>  " ${your_output}

  if [[ $engine_output -eq $your_output ]]; then
    echo "PASSED ✅"
  else
    echo "FAILED ❌"
    echo "${fen1}"
    echo "${fen2}"
  fi
  IFS=''
}

if [[ -n $tempfen ]]; then
  test_fen "${tempfen}" " "
else
  while read fen
  do
    test_fen "${fen}" " "
  done < "${fenfile}"
fi
