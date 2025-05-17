CC = cc
CFLAGS = -Wall -g

all: main hello

main: main.o
	$(CC) main.o -o main

hello: hello.o
	$(CC) hello.o -o hello

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

hello.o: hello.c
	$(CC) $(CFLAGS) -c hello.c -o hello.o

clean:
	rm -f *.o main hello

.PHONY: all clean
