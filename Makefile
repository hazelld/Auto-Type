CC=gcc
CFLAGS= -Wall -std=c99

auto_type: auto_type.o cinput.o main.o

auto_type.o: auto_type.c auto_type.h

cinput.o: cinput.c cinput.h

main.o: main.c

clean:
	rm -rf auto_type *.o
