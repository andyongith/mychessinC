
# My Chess in C
A text-based chess game entirely made in C

## What?
```
$ ./start white  
FEN: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1  
  +---+---+---+---+---+---+---+---+  
8 | r | n | b | q | k | b | n | r |  
  +---+---+---+---+---+---+---+---+  
7 | p | p | p | p | p | p | p | p |  
  +---+---+---+---+---+---+---+---+  
6 |   |   |   |   |   |   |   |   |  
  +---+---+---+---+---+---+---+---+  
5 |   |   |   |   |   |   |   |   |  
  +---+---+---+---+---+---+---+---+  
4 |   |   |   |   |   |   |   |   |    
  +---+---+---+---+---+---+---+---+  
3 |   |   |   |   |   |   |   |   |  
  +---+---+---+---+---+---+---+---+  
2 | P | P | P | P | P | P | P | P |  
  +---+---+---+---+---+---+---+---+  
1 | R | N | B | Q | K | B | N | R |  
  +---+---+---+---+---+---+---+---+  
    a   b   c   d   e   f   g   h  
num_of_available_moves: 20  
Enter your move: 
```

## Why?
* For fun
* I just learned C and wanted to build some project in it to learn more
* Also I like playing chess, here's [my chess.com profile](https://chess.com/member/andy_on_chess) if you're interested

## How?(for users)
First compile it using the command
```
make
```
**note: You must have gcc compiler(_you can use other compilers too but only if you know what you're doing_)**  
Now to use it, run
```
./start [options] [side]
```
if you ommit the _side_ it'll assume you're playing as white
### side
* **white**  
 simply, you play as white i.e bot will play as black
* **black**  
 simply, you play as black i.e bot will play as white
* **both**  
 you're playing from both sides i.e no bot moves
* **random**
 you'll do nothing but just see i.e bot will play from both side
### available options
* **-f <your_fen>**  
 You can use this option to set up your desired fen(or should I say position). However in place of _<your_fen>_ you can also use
  * **startfen** for starting position
  * **testfen <index>** for _testFen[index]_
  * **ltestfen** for _testFen[last index]_  
 ##### example
 ```
$ ./start -f "8/R1P5/4p3/1P2k3/8/2r5/5PK1/8 w - - 1 45"
FEN: 8/R1P5/4p3/1P2k3/8/2r5/5PK1/8 w - - 1 45
  +---+---+---+---+---+---+---+---+
8 |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
7 | R |   | P |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
6 |   |   |   |   | p |   |   |   |
  +---+---+---+---+---+---+---+---+
5 |   | P |   |   | k |   |   |   |
  +---+---+---+---+---+---+---+---+
4 |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
3 |   |   | r |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
2 |   |   |   |   |   | P | K |   |
  +---+---+---+---+---+---+---+---+
1 |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
    a   b   c   d   e   f   g   h
num_of_available_moves: 16
Enter your move:
 ```
* **-l**  
 Use this option if you want it to always show all the legal moves even if it's the bot's turn.
* **-d <time_in_sec>**  
 Use this option to set delay between bot moves
* **-h**  
 Use this option for any help

## How?(for devs)
Just fork it and do whatever you want

## Got a suggestion or issues?
**note: before suggesting me anything first look at my [To-do](#to-do) list**  
[Go to issues](https://github.com/andyongith/mychessinC/issues)

## To-do
#### [Issues]
* Fix bug related to en passant and others(if any)
#### [Features]
* Add unicode symbols(enable using -u option) for chess pieces
* Add the ability to flip the board
* Make a better help(-h option) prompt
* Make a better README

## What to do if you don't appreciate my work
* Fuck off
