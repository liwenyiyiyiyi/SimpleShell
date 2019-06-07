CC=gcc
CFLAGS=-Wall -Wpedantic -Wextra -lreadline -std=gnu89


shell: shell.o
	gcc $(CFLAGS) -o shell shell.o

shell.o: shell.c parse.o
	gcc  $(CFLAGS) -c -o $@ $<

parse.o: parse.c parse.h
	gcc  $(CFLAGS) -c -o $@ $<

clean:
	rm -f shell *.o