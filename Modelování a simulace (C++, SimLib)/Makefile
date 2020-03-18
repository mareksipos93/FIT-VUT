# Makefile IMS Marek Sipos (xsipos03)
CC=g++
CFLAGS=-std=c++98 -Wall -Wextra -pedantic -lsimlib -lm
LFLAGS=-c

RM=rm -f

all: main

main: main.o
	$(CC) $(CFLAGS) main.o -o main
main.o:
	$(CC) $(LFLAGS) main.cpp

.PHONY:
	clean

clean:
	${RM} *.o main
run:
	./main