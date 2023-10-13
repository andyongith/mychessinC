CC=gcc
CFLAGS=

start: main.o position.o display.o legalmoves.o miscfunctions.o
	$(CC) main.o position.o display.o legalmoves.o miscfunctions.o -o $@

main.o: main.c position.h display.h legalmoves.h
	$(CC) $(CFLAGS) main.c -c

position.o: position.c position.h
	$(CC) $(CFLAGS) position.c -c

display.o: display.c display.h position.h
	$(CC) $(CFLAGS) display.c -c

legalmoves.o: legalmoves.c position.h
	$(CC) $(CFLAGS) legalmoves.c -c

miscfunctions.o: miscfunctions.c miscfunctions.h position.h
	$(CC) $(CFLAGS) miscfunctions.c -c

clean:
	$(RM) *.o start
