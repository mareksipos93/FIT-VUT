#
# makefile
# Solution: IJC-DU1, task b), 25.03 2016
# Author: Marek Sipos (xsipos03), FIT VUT (BUT)
#

CC=gcc
CFLAGS=-O2 -std=c99 -Wall -Wextra -pedantic -lm

all: primes primes-inline steg-decode

primes: primes.o eratosthenes.o error.o
	$(CC) $(CFLAGS) primes.o eratosthenes.o error.o -lm -o $@
primes.o: primes.c bit-array.h eratosthenes.h
	$(CC) $(CFLAGS) -c primes.c -o $@
eratosthenes.o: eratosthenes.c bit-array.h eratosthenes.h
	$(CC) $(CFLAGS) -c eratosthenes.c -o $@

primes-inline: primes-inline.o eratosthenes-inline.o error.o
	$(CC) $(CFLAGS) primes-inline.o eratosthenes-inline.o error.o -lm -o $@
primes-inline.o: primes.c bit-array.h eratosthenes.h
	$(CC) $(CFLAGS) -DUSE_INLINE -c primes.c -o $@
eratosthenes-inline.o: eratosthenes.c bit-array.h eratosthenes.h
	$(CC) $(CFLAGS) -DUSE_INLINE -c eratosthenes.c -o $@

steg-decode: steg-decode.o eratosthenes.o error.o ppm.o
	$(CC) $(CFLAGS) steg-decode.o eratosthenes.o error.o ppm.o -lm -o $@
steg-decode.o: steg-decode.c eratosthenes.h error.h ppm.h
	$(CC) $(CFLAGS) -c steg-decode.c -o $@

bit-array.h: error.o
error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c -o $@
ppm.o: ppm.c ppm.h error.h
	$(CC) $(CFLAGS) -c ppm.c -o $@

.PHONY:
	clean clean-all

clean:
	rm -f *.o

clean-all:
	rm -f *.o primes primes-inline steg-decode steg-encode