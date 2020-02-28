build:	poker_tournament.o
	gcc -Wall poker_tournament.o -o poker_tournament

poker_tournament.o: poker_tournament.c info.h
	gcc -Wall -c poker_tournament.c -o poker_tournament.o

clean:
	rm -rf poker_tournament	poker_tournament.o