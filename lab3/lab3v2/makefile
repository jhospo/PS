CC = gcc
CFLAGS = -Wall -Wextra -g -fPIC
LDFLAGS = -ldl



all: show_users

groups.o: groups.c groups.h
	$(CC) $(CFLAGS) -c  groups.c

libgroups.so: groups.o 
	$(CC) -shared -o  libgroups.so groups.o

main.o: main.c groups.h
	$(CC) $(CFLAGS) -c main.c

show_users: main.o libgroups.so
	$(CC) -o show_users main.o $(LDFLAGS)

clean:
	rm -f *.o show_users libgroups.a
