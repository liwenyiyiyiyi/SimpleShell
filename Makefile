CC=gcc
CFLAGS=-g -Wall

all: clean shell

shell: shell.c parse.c parse.h
	$(CC) $(CFLAGS) -o shell shell.c parse.c

clean:
	$(RM) -rf shell *.PHONY
