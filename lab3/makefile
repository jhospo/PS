C = gcc
CFLAGS = -Wall -Wextra -g

all: show_users

show_users: main.o libgroups.a
	$(CC) $(CFLAGS) -o show_users main.o -L. -lgroups

main.o: main.c groups.h
	$(CC) $(CFLAGS) -c main.c

groups.o: groups.c groups.h
	$(CC) $(CFLAGS) -c groups.c

libgroups.a: groups.o
	ar rcs libgroups.a groups.o

clean:
	rm -f *.o show_users libgroups.a
