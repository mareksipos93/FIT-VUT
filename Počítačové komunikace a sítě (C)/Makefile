default: chat_client

chat_client.o: chat_client.c
	gcc -Wall -pedantic -std=c99 -c chat_client.c -o chat_client.o -pthread

chat_client: chat_client.o
	gcc -Wall -pedantic -std=c99 chat_client.o -o chat_client -pthread

.PHONY: clean

clean:
	-rm -f chat_client.o
	-rm -f chat_client
