CC = gcc
CFLAGS = -Wall -pthread
OBJS = main.o timer.o
TARGET = threads

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

main.o: main.c timer.h
	$(CC) $(CFLAGS) -c main.c

timer.o: timer.c timer.h
	$(CC) $(CFLAGS) -c timer.c

clean:
	rm -f *.o $(TARGET)
