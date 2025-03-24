CC = gcc
CFLAGS = -Wall -Wextra -g

all: test_menager

test_menager: test_mena.o mem_mena.a
	$(CC) $(CFLAGS) -o test_menager test_mena.o mem_mena.a

test_mena.o: main.c mem_mena.h
	$(CC) $(CFLAGS) -c main.c -o test_mena.o

mem_mena.o: mem_mena.c mem_mena.h
	$(CC) $(CFLAGS) -c mem_mena.c -o mem_mena.o

mem_mena.a: mem_mena.o
	ar rcs mem_mena.a mem_mena.o

clean:
	rm -f *.o test_menager mem_mena.a
