CC=gcc
CFLAGS=-Wall -g -s -fsanitize=address -pthread

mssv: mssv.o validate.o
	$(CC) $(CFLAGS) -o mssv mssv.o validate.o

mssv.o: mssv.c sudoku.h
	$(CC) $(CFLAGS) -c mssv.c

validate.o: validate.c sudoku.h 
	$(CC) $(CFLAGS) -c validate.c

clean:
	rm -f *.o mssv
