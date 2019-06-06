CC=gcc
CFLAGS=-Wall -Wpedantic -Wextra -lreadline -std=gnu89

TARGET=shell

all: $(TARGET)

$(TARGET): shell.c parse.o
	gcc ${CFLAGS} $^ -o $@

parse.o: parse.c parse.h
	${CC} ${CFLAGS} -c $< -o $@


clean:
	rm -f *.o ${TARGET}

