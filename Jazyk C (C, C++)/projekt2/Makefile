#
# Makefile
# Solution: IJC-DU2, task a) + b), 25.03 2016
# Author: Marek Sipos (xsipos03), FIT VUT (BUT)
#

CC=gcc
CCPP=g++
CFLAGS=-std=c99 -g -pedantic -Wall -Wextra
CPPFLAGS=-std=c++11 -g -pedantic -Wall -Wextra

all: tail tail2 wordcount wordcount-dynamic

tail: tail.o
	$(CC) $(CFLAGS) tail.o -o $@
tail.o: tail.c
	$(CC) $(CFLAGS) -c tail.c -o $@

tail2: tail2.o
	$(CCPP) $(CPPFLAGS) tail2.o -o $@
tail2.o: tail2.cc
	$(CCPP) $(CPPFLAGS) -c tail2.cc -o $@

wordcount: wordcount.o io.o htable.a
	$(CC) $(CFLAGS) -o $@ -static $^
wordcount-dynamic: wordcount.o io.o htable.so
	$(CC) -fPIC $(CFLAGS) -o $@ $^

htable.a: htab_init.o htab_init2.o htab_lookup_add.o htab_foreach.o htab_remove.o htab_clear.o htab_free.o hash_function.o
	ar crs $@ $^
htable.so: htab_init.o htab_init2.o htab_lookup_add.o htab_foreach.o htab_remove.o htab_clear.o htab_free.o hash_function.o
	gcc $(CFLAGS) -shared -o $@ $^

htab_init.o: htab_init.c htable.h
	$(CC) -fPIC $(CFLAGS) -c htab_init.c -o $@
htab_init2.o: htab_init2.c htable.h
	$(CC) -fPIC $(CFLAGS) -c htab_init2.c -o $@
htab_lookup_add.o: htab_lookup_add.c htable.h
	$(CC) -fPIC $(CFLAGS) -c htab_lookup_add.c -o $@
htab_foreach.o: htab_foreach.c htable.h
	$(CC) -fPIC $(CFLAGS) -c htab_foreach.c -o $@
htab_remove.o: htab_remove.c htable.h
	$(CC) -fPIC $(CFLAGS) -c htab_remove.c -o $@
htab_clear.o: htab_clear.c htable.h
	$(CC) -fPIC $(CFLAGS) -c htab_clear.c -o $@
htab_free.o: htab_free.c htable.h
	$(CC) -fPIC $(CFLAGS) -c htab_free.c -o $@
hash_function.o: hash_function.c htable.h
	$(CC) -fPIC $(CFLAGS) -c hash_function.c -o $@

.PHONY:
	all clean clean-all zip

clean:
	rm -f *.o

clean-all:
	rm -f *.o tail tail2 wordcount wordcount-dynamic htable.a htable.so

zip:
	zip xsipos03.zip *.c *.cc *.h Makefile
