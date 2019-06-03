CC=gcc
CFLAGS=-Wall -Wpedantic -Wextra -Werror -lreadline -std=c89

TARGET=shell

all: $(TARGET)

$(TARGET): shell.c parse.o
	${CC} ${CFLAGS} $^ -o $@

parse.o: parse.c parse.h
	${CC} ${CFLAGS} -c $< -o $@

.PHONY: clean dist

clean:
	rm -f *.o ${TARGET}

dist:
	tar cf project4.tar parse.c parse.h shell.c
