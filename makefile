CC=gcc
CFLAGS=

SRC=src

start: main.o position.o display.o legalmoves.o miscfunctions.o
	$(CC) main.o position.o display.o legalmoves.o miscfunctions.o -o $@

main.o: $(SRC)/main.c $(SRC)/position.h $(SRC)/display.h $(SRC)/legalmoves.h
	$(CC) $(CFLAGS) $(SRC)/main.c -c

position.o: $(SRC)/position.c $(SRC)/position.h
	$(CC) $(CFLAGS) $(SRC)/position.c -c

display.o: $(SRC)/display.c $(SRC)/display.h $(SRC)/position.h
	$(CC) $(CFLAGS) $(SRC)/display.c -c

legalmoves.o: $(SRC)/legalmoves.c $(SRC)/position.h
	$(CC) $(CFLAGS) $(SRC)/legalmoves.c -c

miscfunctions.o: $(SRC)/miscfunctions.c $(SRC)/miscfunctions.h $(SRC)/position.h
	$(CC) $(CFLAGS) $(SRC)/miscfunctions.c -c

clean:
	$(RM) *.o start
